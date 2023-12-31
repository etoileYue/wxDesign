#include "Pin.h"

Pin::Pin() {}

Pin::Pin(int pinId, wxPoint pos) {
	this->pinId = pinId;
	this->pinPos = pos;
}