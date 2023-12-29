#include "Read.h"
#include <wx/dirdlg.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/filename.h>


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

        // ���������š����˵�
        menuSymbol = new wxMenu;

        // �������š����˵���ӵ��˵���
        menuBar->Append(menuSymbol, wxT("����"));

        // ���˵������õ�����
        SetMenuBar(menuBar);

        // �󶨲˵��¼�������

        Bind(wxEVT_MENU, &DrawFrame::OnOpen, this, wxID_OPEN);
        Bind(wxEVT_MENU, &DrawFrame::OnSave, this, wxID_SAVE);
        Bind(wxEVT_PAINT, &DrawFrame::OnPaint, this);

        Bind(wxEVT_LEFT_DOWN, &DrawFrame::OnLeftDown, this);
        Bind(wxEVT_LEFT_UP, &DrawFrame::OnLeftUp, this);
        Bind(wxEVT_MOTION, &DrawFrame::OnMouseMove, this);

    }

private:

    vector<Sign> signs;
    vector<Sign> SelectedSigns;

    wxMenu* menuSymbol;  // �������˵�

    void OnOpen(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, "ѡ���ļ���", "", "", "*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return;  // �û����ȡ��
        }

        wxString path = openFileDialog.GetPath();

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
        //todo
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

        Refresh();
    }
    void OnLeftDown(wxMouseEvent& event) {
        for (auto& sign : SelectedSigns) {
            if (sign.isInSelectArea(event)) {
                sign.is_dragging = TRUE;
            }
        }
    }

    void OnLeftUp(wxMouseEvent& event) {
        for (auto& sign : SelectedSigns) {
            sign.is_dragging = FALSE;
        }
    }

    void OnMouseMove(wxMouseEvent& event) {
        for (auto& sign : SelectedSigns) {
            if (sign.is_dragging) {
                sign.SetPos(event);
                Refresh();
                break;
            }
        }
    }

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);

        for (auto& sign : SelectedSigns) {
            sign.DrawSign(dc);
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

