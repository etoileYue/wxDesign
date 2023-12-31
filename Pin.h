#pragma once
#include <wx/wx.h>

class Pin {
public:
	int pinId;
	wxPoint pinPos;

	Pin();
	Pin(int pinId, wxPoint pinpos);
};