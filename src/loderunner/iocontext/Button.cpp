#include "Button.h"

#include <time.h>

void Button::setImpulseTime(float time) {
	impulseTime = time;
}

void Button::detect(bool physPush) {
	if (pushed && !physPush) {
		release = true;
	}		

	pushed = physPush;
}

bool Button::simple() {	
	if (pushed) {
		if (!simplePushed) {
			simplePushed = true;
			return true;
		}

		return false;
	}

	simplePushed = false;
	return false;
}

bool Button::impulse() {
	if (pushed) {
		float curClock = ((float)clock() / CLOCKS_PER_SEC);

		if (curClock - lastImpulse > impulseTime) {
			lastImpulse = curClock;
			return true;
		}
	}

	return false;
}

bool Button::continuous() {
	return pushed;
}

bool Button::released() {
	return release;
}
