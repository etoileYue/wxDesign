#include "Sign.h"

Sign::Sign() {};

Sign& Sign::operator = (Sign const& sign) {
    this->id = sign.id;
    this->name = sign.name;
    this->lines = sign.lines;
    this->arcs = sign.arcs;
    this->rectangles = sign.rectangles;
    this->circles = sign.circles;
    this->texts = sign.texts;
    this->polygons = sign.polygons;
    this->pos = sign.pos;
    
    return *this;
}

Sign::Sign(Json::Value root) {
    this->is_dragging = FALSE;

    id = root["id"].asInt();
    name = root["name"].asString();

    Json::Value composition = root["composition"];
    Json::Value lines = composition["line"];
    Json::Value arcs = composition["arc"];
    Json::Value rectangles = composition["rectangle"];
    Json::Value circles = composition["circle"];
    Json::Value texts = composition["text"];
    Json::Value polygons = composition["polygon"];

    for (int i = 0; i < 2; i++) {
        this->pos.push_back(root["pos"][i].asInt());
    }

    for (int i = 0; i < 2; i++) {
        this->SelectedRect.push_back(root["Selected"][i].asInt());
    }

    int count = 0;

    for (int i = 0; i < root["pin"].size()/2; i++) {
        count++;
        wxPoint pos = wxPoint(root["pin"][2 * i].asInt(), root["pin"][2 * i + 1].asInt());
        Pin pin = Pin(count, pos);
        pins.push_back(pin);
    }

    for (auto& line : lines) {
        this->lines.push_back(line);
    }

    for (auto& arc : arcs) {
        this->lines.push_back(arc);
    }

    for (auto& rectangle : rectangles) {
        this->rectangles.push_back(rectangle);
    }

    for (auto& circle : circles) {
        this->lines.push_back(circle);
    }

    for (auto& text : texts) {
        this->texts.push_back(text);
    }
}

void Sign::DrawLines(wxPaintDC& dc) {
    for (auto& line : lines) {
        auto pen = line["pen"];
        auto relpos = line["relpos"];
        vector<int> pos;

        for (int i = 0; i < relpos.size(); i++) {
            int realpos = relpos[i].asInt() + this->pos[i%2];
            pos.push_back(realpos);
        }

        dc.SetPen(wxPen(wxColor(pen["color"].asString()), pen["width"].asInt()));
        dc.DrawLine(wxPoint(pos[0], pos[1]), wxPoint(pos[2], pos[3]));
    }
}

void Sign::DrawArcs(wxPaintDC& dc) {
    for (auto& arc : arcs) {
        auto pen = arc["pen"];
        auto relpos = arc["relpos"];
        vector<int> pos;

        for (int i = 0; i < relpos.size(); i++) {
            pos.push_back(relpos[i].asInt() + this->pos[i%2]);
        }

        dc.SetPen(wxPen(wxColor(pen["color"].asString()), pen["width"].asInt()));
        dc.DrawArc(wxPoint(pos[0], pos[1]), wxPoint(pos[2], pos[3]), wxPoint(pos[4], pos[5]));
    }
}

void Sign::DrawRectangles(wxPaintDC& dc) {
    for (auto& rectangle : rectangles) {
        auto pen = rectangle["pen"];
        auto brush = rectangle["brush"];
        auto relpos = rectangle["relpos"];
        vector<int> pos;

        for (int i = 0; i < 2; i++) {
            pos.push_back(relpos[i].asInt() + this->pos[i%2]);
        }

        dc.SetPen(wxPen(wxColor(pen["color"].asString()), pen["width"].asInt()));
        dc.SetBrush(wxBrush(wxColor(brush.asString())));
        dc.DrawRectangle(pos[0], pos[1], relpos[2].asInt(), relpos[3].asInt());
    }
}

void Sign::DrawCircles(wxPaintDC& dc) {
    for (auto& circle : circles) {
        auto pen = circle["pen"];
        auto brush = circle["brush"];
        auto relpos = circle["relpos"];
        auto radius = circle["radius"];
        vector<int> pos;

        for (int i = 0; i < 2; i++) {
            pos.push_back(relpos[i].asInt() + this->pos[i]);
        }

        dc.SetPen(wxPen(wxColor(pen["color"].asString()), pen["width"].asInt()));
        dc.SetBrush(wxBrush(wxColor(brush.asString())));
        dc.DrawCircle(wxPoint(pos[0], pos[1]), radius.asInt());
    }
}

void Sign::DrawPolygons(wxPaintDC& dc) {
    //todo
}

void Sign::DrawTexts(wxPaintDC& dc) {
    for (auto& text : texts) {
        auto color = text["color"];
        auto relpos = text["relpos"]; 
        vector<int> pos;

        for (int i = 0; i < relpos.size(); i++) {
            pos.push_back(relpos[i].asInt() + this->pos[i%2]);
        }

        dc.SetTextForeground(wxColor(color.asString()));
        dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.DrawText(text["content"].asString(), wxPoint(pos[0], pos[1]));
    }
}

void Sign::DrawSign(wxPaintDC& dc) {
    this->DrawLines(dc);
    this->DrawArcs(dc);
    this->DrawRectangles(dc);
    this->DrawCircles(dc);
    this->DrawPolygons(dc);
    this->DrawTexts(dc);
}

bool Sign::isInSelectArea(wxMouseEvent& event) {
    wxPoint mousePos = event.GetPosition();

    wxRect rect(this->pos[0], this->pos[1], this->SelectedRect[0], this->SelectedRect[1]);
    if (rect.Contains(mousePos))
        return TRUE;
    return FALSE;
}

int Sign::isInPinArea(wxMouseEvent& event) {
    wxPoint mousePos = event.GetPosition();

    for (auto& pin : pins) {

        wxRect rect(this->pos[0]+pin.pinPos.x-5 , this->pos[1]+pin.pinPos.y-5 , 10, 10);
        if (rect.Contains(mousePos))
            return pin.pinId;
    }
    return -1;
    
}

void Sign::SetPos(wxMouseEvent& event, int deltax, int deltay) {
    wxPoint mousePos = event.GetPosition();

    this->pos[0] = mousePos.x - deltax;
    this->pos[1] = mousePos.y - deltay;

}

string Sign::getName() {
    return this->name;
}

void Sign::setName(string Signname) {
    this->name = Signname;
}

int Sign::getID() {
    return this->id;
}

int Sign::getPos(int index) {
    return this->pos[index];
}

wxPoint Sign::getPinPos(int pinId) {
    for (auto& pin : pins) {
        if (pin.pinId == pinId) {
            wxPoint pos = wxPoint(this->pos[0]+pin.pinPos.x, this->pos[1]+pin.pinPos.y);
            return pos;
        }
    }
    return wxPoint();
}

void Sign::setConnect(int pinId1, string Signname, Pin pin2) {
    Connected connect{ pinId1, Signname, pin2 };
    this->connected.push_back(connect);
}

Pin* Sign::getPin(int pinId) {
    for (auto& pin : pins) {
        if (pin.pinId == pinId)
            return &pin;
    }
}

vector<Pin*> Sign::getAllPin() {
    vector<Pin*> pins;
    for (auto& pin : this->pins) {
        pins.push_back(&pin);
    }
    return pins;
}