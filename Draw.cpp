#include "Sign.h"
#include <wx/dirdlg.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include "Save.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>


class DrawFrame : public wxFrame
{
public:
    DrawFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
    {

        wxMenuBar* menuBar = new wxMenuBar;

        wxMenu* menuFile = new wxMenu;
        menuFile->Append(wxID_OPEN, "打开文件\tctrl+O");
        menuFile->Append(wxID_SAVE, "保存\tctrl+S");

        // 将“文件”主菜单添加到菜单栏，排在前面
        menuBar->Append(menuFile, wxT("文件"));

        wxMenu* menuOper = new wxMenu;
        menuOper->Append(ID_DRAWLINE, "连接电路");
        menuOper->Append(ID_MOVESIGN, "移动元件");
        menuBar->Append(menuOper, wxT("操作"));

        // 创建“符号”主菜单
        menuSymbol = new wxMenu;

        // 将“符号”主菜单添加到菜单栏
        menuBar->Append(menuSymbol, wxT("符号"));

        // 将菜单栏设置到窗口
        SetMenuBar(menuBar);

        // 绑定菜单事件处理函数
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
    Sign* currentSign;
    int currentPinId;
    
    struct Line {
        wxPoint start;
        wxPoint end;
    };
    
    vector<Line> lines;
    wxPoint start, end, current;
    bool is_drawing;

    enum {
        ID_MOVESIGN = 100,
        ID_DRAWLINE = 101
    };
    int OperFlag, deltax, deltay;

    string libpath;

    wxMenu* menuSymbol;  // 符号主菜单

    void OnOpen(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, "选择文件打开", "", "", "*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return;  // 用户点击取消
        }

        wxString path = openFileDialog.GetPath();

        libpath = path.ToStdString();

        ifstream file(path.ToStdString());  // 将wxString转换为std::string
        if (!file.is_open()) {
            // 文件打开失败，处理错误...
            return;
        }

        Json::Value root;  // 用于存储解析后的JSON数据
        Json::Reader reader;

        if (!reader.parse(file, root)) {
            // JSON解析失败，处理错误...
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
        wxFileDialog saveFileDialog(this, "选择文件保存", "", "", "*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        
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
        // 添加符号菜单项
        wxMenuItem* symbolItem = menuSymbol->Append(symbolID, symbolName);

        Bind(wxEVT_MENU, &DrawFrame::OnSymbolMenuItemSelected, this, symbolItem->GetId());
    }

    void OnSymbolMenuItemSelected(wxCommandEvent& event) {

        for (auto& sign : signs) {
            if (sign.getID() == event.GetId()) {
                SelectedSigns.push_back(sign);
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
                    currentSign = &sign;
                    currentPinId = pinId;
                }
            }
        }
        
    }

    void OnLeftUp(wxMouseEvent& event) {
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
                        lines.push_back({ start, end });
                        currentSign->setConnect(currentPinId, sign.getName(), sign.getPin(pinId));
                        sign.setConnect(pinId, currentSign->getName(), currentSign->getPin(currentPinId));
                    }
                }
                Refresh();
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
                Refresh();
            }
        }
        
    }

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);

        for (auto& sign : SelectedSigns) {
            sign.DrawSign(dc);
        }

        dc.SetPen(wxPen(wxColor(0, 0, 0)));
        for (auto& line : lines) {
            dc.DrawLine(line.start.x, line.start.y, line.end.x, line.end.y);
        }

        if (is_drawing) {
            dc.SetPen(wxPen(wxColour(0, 0, 0)));
            dc.DrawLine(start.x, start.y, current.x, current.y);
        }
   
    } 

    wxDECLARE_EVENT_TABLE();

};          

BEGIN_EVENT_TABLE(DrawFrame, wxFrame)
EVT_PAINT(DrawFrame::OnPaint)
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