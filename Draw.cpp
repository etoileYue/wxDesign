#include "Sign.h"
#include <wx/dirdlg.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include "Save.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include "Wire.h"



class DrawFrame : public wxFrame
{
public:
    DrawFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
    {

        wxMenuBar* menuBar = new wxMenuBar;

        wxMenu* menuFile = new wxMenu;
        menuFile->Append(wxID_OPEN, "���ļ�\tctrl+O");
        menuFile->Append(wxID_SAVE, "����\tctrl+S");

        // �����ļ������˵���ӵ��˵���������ǰ��
        menuBar->Append(menuFile, wxT("�ļ�"));

        wxMenu* menuOper = new wxMenu;
        menuOper->Append(ID_DRAWLINE, "���ӵ�·");
        menuOper->Append(ID_MOVESIGN, "�ƶ�Ԫ��");
        menuBar->Append(menuOper, wxT("����"));

        // ���������š����˵�
        menuSymbol = new wxMenu;

        // �������š����˵���ӵ��˵���
        menuBar->Append(menuSymbol, wxT("����"));

        // ���˵������õ�����
        SetMenuBar(menuBar);

        // �󶨲˵��¼�������
        OperFlag = ID_MOVESIGN;
        is_drawing = FALSE;

        Bind(wxEVT_MENU, &DrawFrame::OnOpen, this, wxID_OPEN);
        Bind(wxEVT_MENU, &DrawFrame::OnSave, this, wxID_SAVE);
        Bind(wxEVT_MENU, &DrawFrame::Sign2Line, this, ID_DRAWLINE);
        Bind(wxEVT_MENU, &DrawFrame::Line2Sign, this, ID_MOVESIGN);
        Bind(wxEVT_PAINT, &DrawFrame::OnPaint, this);
        
        Bind(wxEVT_LEFT_DOWN, &DrawFrame::OnLeftDown, this);
        Bind(wxEVT_LEFT_UP, &DrawFrame::OnLeftUp, this);
        Bind(wxEVT_MOTION, &DrawFrame::OnMouseMove, this);

        SetBackgroundColour(*wxWHITE);

    }

private:

    vector<Sign> signs;
    vector<Sign> SelectedSigns;

    vector<Wire> wires;
    Wire* currentwire;

    wxPoint start, end, current;
    bool is_drawing;

    enum {
        ID_MOVESIGN = 100,
        ID_DRAWLINE = 101
    };
    int OperFlag, deltax, deltay;

    string libpath;

    wxMenu* menuSymbol;  // �������˵�

    void OnOpen(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, "ѡ���ļ���", "", "", "*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return;  // �û����ȡ��
        }

        wxString path = openFileDialog.GetPath();

        libpath = path.ToStdString();

        ifstream file(path.ToStdString());  // ��wxStringת��Ϊstd::string
        if (!file.is_open()) {
            // �ļ���ʧ�ܣ��������...
            return;
        }

        Json::Value root;  // ���ڴ洢�������JSON����
        Json::Reader reader;

        if (!reader.parse(file, root)) {
            // JSON����ʧ�ܣ��������...
            return;
        }

        if (root.isArray()) {
            for (const auto& each : root) {
                Sign sign(each);
                this->signs.push_back(sign);
            }
        }
        else {
            Sign sign(root);
            this->signs.push_back(sign);
        }

        wxMenuItemList items = menuSymbol->GetMenuItems();
        for (wxMenuItem* item : items) {
            menuSymbol->Delete(item->GetId());
        }

        for (auto& sign : this->signs) {
            AddSymbolMenuItem(sign.getName(), sign.getID());
        }
    }

    void OnSave(wxCommandEvent& event) {
        wxFileDialog saveFileDialog(this, "ѡ���ļ�����", "", "", "*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        
        vector<string> sthelse;
        string path = saveFileDialog.GetPath().ToStdString();     
        
        auto now = std::chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        ostringstream oss;
        oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
        string date = oss.str();

        string title = saveFileDialog.GetTitle().ToStdString();

        sthelse.push_back(path);
        sthelse.push_back(date);
        sthelse.push_back(title);
        sthelse.push_back(libpath);

        string str = SaveJson(SelectedSigns, sthelse);

        if (saveFileDialog.ShowModal()) {
            wxString path = saveFileDialog.GetPath();

            wxFileOutputStream outputstream(path);

            if (wxFile::Access(path, wxFile::write)) {
                wxFile* file = outputstream.GetFile();
                file->Write(str);
                file->Close();
            }
        }

    }

    void Sign2Line(wxCommandEvent& event) {
        OperFlag = ID_DRAWLINE;
    }

    void Line2Sign(wxCommandEvent& event) {
        OperFlag = ID_MOVESIGN;
    }
    
    void AddSymbolMenuItem(const wxString& symbolName, const int symbolID) {
        // ��ӷ��Ų˵���
        wxMenuItem* symbolItem = menuSymbol->Append(symbolID, symbolName);

        Bind(wxEVT_MENU, &DrawFrame::OnSymbolMenuItemSelected, this, symbolItem->GetId());
    }

    void OnSymbolMenuItemSelected(wxCommandEvent& event) {

        for (auto& sign : signs) {
            if (sign.getID() == event.GetId()) {
                SelectedSigns.push_back(sign);              
            }
        }
        for (auto& sign : SelectedSigns) {
            for (Pin* pin : sign.getAllPin()) {
                Wire wire(pin);
                wires.push_back(wire);
                pin->setSign(&sign);
            }
        }
        
        for (auto& wire : wires) {
            for (Pin& pin : wire.pins) {
                pin.setWire(&wire);
            }
        }
        Refresh();
      
    }

    void OnLeftDown(wxMouseEvent& event) {
        if (OperFlag == ID_MOVESIGN) {
            for (auto& sign : SelectedSigns) {
                if (sign.isInSelectArea(event)) {
                    deltax = event.GetPosition().x - sign.getPos(0);
                    deltay = event.GetPosition().y - sign.getPos(1);
                    sign.is_dragging = TRUE;
                }
            }
        }
        else if (OperFlag == ID_DRAWLINE) {
            for (auto& sign : SelectedSigns) {
                int pinId = sign.isInPinArea(event);              
                if (pinId != -1) {
                    is_drawing = TRUE;
                    start = sign.getPinPos(pinId);
                    current = start;
                    Pin* pintest = sign.getPin(pinId);
                    currentwire = pintest->wire;
                }

            }
            for (auto& wire : wires) {
                vector<Node> nodes = wire.nodes;
                for (auto& node : nodes) {
                    if (node.isInNodeArea(event)) {
                        start = node.getPos();
                        current = start;
                        is_drawing = TRUE;
                        currentwire = node.wire;
                    }
                }
            }
            
        }
        
    }

    void OnLeftUp(wxMouseEvent& event) {
        bool inBlank = TRUE;
        if (OperFlag == ID_MOVESIGN) {
            for (auto& sign : SelectedSigns) {
                sign.is_dragging = FALSE;
            }
        }
        else if (OperFlag == ID_DRAWLINE) {
            if (is_drawing) {
                for (auto& sign : SelectedSigns) {
                    int pinId = sign.isInPinArea(event);
                    if (pinId != -1) {
                        end = sign.getPinPos(pinId);
                        sign.getPin(pinId)->wire->addLine(Line(start, end));
                        inBlank = FALSE;
                        currentwire->mergeWire(sign.getPin(pinId)->wire);
                        currentwire->setConnected();
                    }
                }
                for (auto& wire : wires) {
                    vector<Node> nodes = wire.nodes;
                    for (auto& node : nodes) {
                        int nodeId = node.isInNodeArea(event);
                        if (nodeId != -1) {
                            end = node.getPos();
                            node.wire->addLine(Line(start, end));
                            inBlank = FALSE;
                        }
                    }
                }
                
                if (inBlank) {
                    end = current;
                    Node node(end);
                    currentwire->addNode(node);
                    currentwire->addLine(Line(start, end));
                }
                Refresh();
                inBlank = TRUE;
                is_drawing = FALSE;
            }
        }
    }

    void OnMouseMove(wxMouseEvent& event) {
        if (OperFlag == ID_MOVESIGN) {
            for (auto& sign : SelectedSigns) {
                if (sign.is_dragging) {
                    sign.SetPos(event, deltax, deltay);
                    Refresh();
                    break;
                }
            }
        }
        else if (OperFlag == ID_DRAWLINE) {
            if (is_drawing) {
                current = event.GetPosition();
                if (abs(current.x - start.x) > abs(current.y - start.y)) {
                    end.x = current.x;
                    end.y = start.y;
                }
                else {
                    end.x = start.x;
                    end.y = current.y;
                }
                Refresh();
            }
        }
        
    }

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);

        for (auto& sign : SelectedSigns) {
            sign.DrawSign(dc);
        }

        dc.SetPen(wxPen(wxColor(0, 0, 0), 5));

        if (is_drawing) {
            dc.SetPen(wxPen(wxColour(0, 0, 0), 5));
            dc.DrawLine(start.x, start.y, end.x, end.y);
        }
        
        for (auto& wire : wires) {
            wire.drawWire(dc);
        }
    } 

    wxDECLARE_EVENT_TABLE();

};          

BEGIN_EVENT_TABLE(DrawFrame, wxFrame)
EVT_PAINT(DrawFrame::OnPaint)
//EVT_LEFT_DOWN(DrawFrame::OnLeftDown)
//EVT_LEFT_UP(DrawFrame::OnLeftUp)
//EVT_MOTION(DrawFrame::OnMouseMove)
END_EVENT_TABLE()

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        
        DrawFrame* frame = new DrawFrame("Signs");
        frame->Show();
        return true;
    }

    /*auto Sign_init(Json::Value root) {
        if (root.isArray() = true) {

        }
     }*/
};

wxIMPLEMENT_APP(MyApp);