#pragma once
#include <wx/wx.h>
#ifndef NODE_H
#define NODE_H

static int num = 0;

class Wire;

class Node {
public:
	wxPoint pos;
	int nodeId;
	Wire* wire;


	inline Node(wxPoint pos) {
		this->pos.x = pos.x;
		this->pos.y = pos.y;
		this->nodeId = ++num;
	}
	inline Node(int xx, int yy) {
		this->pos = wxPoint(xx, yy);
		this->nodeId = ++num;
	}
	bool isInNodeArea(wxMouseEvent event) {
		wxPoint mousePos = event.GetPosition();
		wxRect rect(pos.x - 5, pos.y - 5, 10, 10);
		if (rect.Contains(mousePos))
			return TRUE;
		return FALSE;
	}
	wxPoint getPos() {
		return pos;
	}

	void setWire(Wire* wirePtr) {
		wire = wirePtr;
	}

};

#endif // !NODE_H