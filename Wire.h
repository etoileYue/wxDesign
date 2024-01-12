#pragma once
#ifndef WIRE_H
#define WIRE_H

#include "Sign.h"
#include "Node.h"
#include "Line.h"

class Wire {
public:
	vector<Pin> pins;
	vector<Node> nodes;
	vector<Line> lines;
	
	Wire(Node* node);
	Wire(Pin* pin);
	void drawWire(wxPaintDC& dc);
	void addNode(Node node);
	void addLine(Line line);
	void addPin(Pin* pin);
	void setConnected();
	void mergeWire(Wire* otherwire);

};
#endif // !WIRE_H