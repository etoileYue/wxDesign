#include "Pin.h"
#include "Sign.h"

void Pin::setSign(Sign* signPtr) {
	sign = signPtr;
}

void Pin::setWire(Wire* wirePtr) {
	wire = wirePtr;
}

Pin::Pin(int pinId, wxPoint pos) {
	this->pinId = pinId;
	this->pinPos = pos;

}
 
bool Pin::operator == (const Pin& otherpin) {
	return(pinId == otherpin.pinId) && (sign == otherpin.sign);
}