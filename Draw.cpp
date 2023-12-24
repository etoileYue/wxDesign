#include "Read.h"

class Board : public wxPanel
{
public:
    Board(wxFrame* parent, vector<Sign> signs) : wxPanel(parent) {
        Bind(wxEVT_PAINT, &Board::OnPaint, this);

        Bind(wxEVT_LEFT_DOWN, &Board::OnLeftDown, this);
        Bind(wxEVT_LEFT_UP, &Board::OnLeftUp, this);
        Bind(wxEVT_MOTION, &Board::OnMouseMove, this);
        this->signs = signs;
    }

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);

        for (auto& sign : signs) {
            sign.DrawSign(dc);
        }
    }

    void OnLeftDown(wxMouseEvent& event) {
        for (auto& sign : signs) {
            if(sign.isInSelectArea(event)){
                sign.is_dragging = TRUE;
            }     
        }
    }

    void OnLeftUp(wxMouseEvent& event) {
        for (auto& sign : signs) {
            sign.is_dragging = FALSE;
        }
    }

    void OnMouseMove(wxMouseEvent& event) {
        for (auto& sign : signs) {
            if (sign.is_dragging) {
                sign.SetPos(event);
                Refresh();
            }
        }
    }
    private:
        vector<Sign> signs;
};


class DrawFrame : public wxFrame
{
public:
    DrawFrame(const wxString& title, vector<Sign> signs) : wxFrame(NULL, wxID_ANY, title)
    {
        Board* board = new Board(this, signs);

    }

    wxDECLARE_EVENT_TABLE();

};

BEGIN_EVENT_TABLE(DrawFrame, wxFrame)
EVT_PAINT(DrawFrame::OnPaint)
END_EVENT_TABLE()

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        Json::Value root;  // 用于存储解析后的JSON数据
        Json::Reader reader;
        vector<Sign> signs;


        ifstream file("C:\\Users\\Lenovo\\Desktop\\test.json");

        reader.parse(file, root);
        
        if (root.isArray()) {
            for (auto& each : root) {
                Sign sign = Sign(each);
                signs.push_back(sign);
            }
        }
        else
        {
            Sign sign = Sign(root);
            signs.push_back(sign);
        }
        
        DrawFrame* frame = new DrawFrame("Signs", signs);
        frame->Show();
        return true;
    }

    /*auto Sign_init(Json::Value root) {
        if (root.isArray() = true) {

        }
     }*/
};

wxIMPLEMENT_APP(MyApp);

