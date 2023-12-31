#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <json/json.h>
#include <fstream>
#include <iostream>
#include <string>
#include <wx/wx.h>
#include "Pin.h"


using namespace std;

class Sign
{
private:
    int id;
    string name;
    vector<int> pos;
    vector<int> SelectedRect;
    vector<Pin> pins;
    vector<Json::Value> lines;
    vector<Json::Value> arcs;
    vector<Json::Value> rectangles;
    vector<Json::Value> polygons;
    vector<Json::Value> circles;
    vector<Json::Value> texts;

public:
    struct Connected {
        int pinId;//该元件对应的pin脚
        string Signname;//连接的元件名
        Pin pin;//连接的元件的pin脚
    };
    vector<Connected> connected;

    bool is_dragging;
    Sign();
    Sign(Json::Value root);
    Sign& operator = (Sign const& sign);

    void DrawLines(wxPaintDC& dc);
    void DrawArcs(wxPaintDC& dc);
    void DrawRectangles(wxPaintDC& dc);
    void DrawPolygons(wxPaintDC& dc);
    void DrawCircles(wxPaintDC& dc);
    void DrawTexts(wxPaintDC& dc);
    void DrawSign(wxPaintDC& dc);
    bool isInSelectArea(wxMouseEvent& event);
    int isInPinArea(wxMouseEvent& event);
    void SetPos(wxMouseEvent& event, int deltax, int deltay);

    string getName();
    void setName(string Signname);
    int getID();
    int getPos(int index);
    Pin getPin(int pinId);
    wxPoint getPinPos(int pinId);
    void setConnect(int pinId1, string Signname, Pin pin2);
};