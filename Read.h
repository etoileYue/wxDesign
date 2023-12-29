#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <json/json.h>
#include <fstream>
#include <iostream>
#include <string>
#include <wx/wx.h>


using namespace std;

class Sign
{
private:
    int id;
    string name;
    vector<int> pos;
    vector<int> SelectedRect;
    vector<Json::Value> lines;
    vector<Json::Value> arcs;
    vector<Json::Value> rectangles;
    vector<Json::Value> polygons;
    vector<Json::Value> circles;
    vector<Json::Value> texts;
    int priority;
    vector<Sign> Connected;

public:
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
    bool isSelected(wxMouseEvent& event);
    void SetPos(wxMouseEvent& event);
    void SetPriority();
    string getName();

};