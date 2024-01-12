#pragma once
#include <wx/wx.h>
#ifndef PIN_H
#define PIN_H

class Sign;
class Wire;

class Pin {
public:
	int pinId;
	wxPoint pinPos;
	Sign* sign;
	Wire* wire;

	Pin(int pinId, wxPoint pos);

	void setSign(Sign* signPtr);
	void setWire(Wire* wirePtr);

	bool operator==(const Pin& otherpin);
};

#endif