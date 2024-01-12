#pragma once
#include <wx/wx.h>

class Line{
public:
	wxPoint start;
	wxPoint end;

	Line(wxPoint start, wxPoint end) {
		this->start = start;
		this->end = end;
	}

	Line(int xx, int xy, int yx, int yy) {
		this->start = wxPoint(xx, xy);
		this->end = wxPoint(yx, yy);
	}
};