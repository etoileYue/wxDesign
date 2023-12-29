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
        menuFile->Append(wxID_OPEN, "打开文件\tctrl+O");
        menuFile->Append(wxID_SAVE, "保存\tctrl+S");

        // 将“文件”主菜单添加到菜单栏，排在前面
        menuBar->Append(menuFile, wxT("文件"));

        // 创建“符号”主菜单
        menuSymbol = new wxMenu;

        // 将“符号”主菜单添加到菜单栏
        menuBar->Append(menuSymbol, wxT("符号"));

        // 将菜单栏设置到窗口
        SetMenuBar(menuBar);

        // 绑定菜单事件处理函数

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

    wxMenu* menuSymbol;  // 符号主菜单

    void OnOpen(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, "选择文件打开", "", "", "*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return;  // 用户点击取消
        }

        wxString path = openFileDialog.GetPath();

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
        //todo
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

