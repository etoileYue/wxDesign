#include "Wire.h"

Wire::Wire(Node* node) {
	this->addNode(*node);
	node->setWire(this);
}

Wire::Wire(Pin* pin) {
	this->addPin(pin);
	pin->setWire(this);
}

void Wire::drawWire(wxPaintDC& dc) {
	dc.SetPen(wxPen(wxColour(0, 0, 0), 5));
	for (auto& line : lines) {
		dc.DrawLine(line.start, line.end);
	}
}

void Wire::addNode(Node node) {
	node.setWire(this);
	Node node1(node.pos.x, node.pos.y);
	this->nodes.push_back(node1);
}

void Wire::addLine(Line line) {
	this->lines.push_back(line);
}

void Wire::addPin(Pin* pin) {
	pin->setWire(this);
	this->pins.push_back(*pin);
}

void Wire::setConnected() {
	for (auto& pin : pins) {
		for (auto& connectedpin : pins) {
			if (pin == connectedpin) {
				continue;
			}
			pin.sign->setConnect(pin.pinId, connectedpin.sign->getName(), connectedpin);
		}
	}
}

void Wire::mergeWire(Wire* otherWire) {
	for (auto& pin : otherWire->pins) {
		this->addPin(&pin);
		pin.setWire(this);
	}

	for (auto& line : otherWire->lines) {
		this->addLine(line);
	}

	for (auto& node : otherWire->nodes) {
		this->addNode(node);
		node.setWire(this);
	}

	delete otherWire;

}