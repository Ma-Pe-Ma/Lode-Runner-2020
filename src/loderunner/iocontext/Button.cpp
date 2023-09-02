#include "Button.h"
#include <iostream>

void Button::setDebounceTime(float time) {
	debounceTime = time;
}

void Button::setImpulseTime(float time) {
	impulseTime = time;
}

void Button::detect(bool physPush) {
	if (physPush) {
		if (!pushed) {
			pushed = true;
			pushStartTime = ((float) clock() / CLOCKS_PER_SEC);
		}
	}
	//handling debounce short time after starting pushing!s
	else {
		if (((float) clock() / CLOCKS_PER_SEC) <= pushStartTime + debounceTime) {
			pushed = true;
		}			
		else {
			pushed = false;
		}			
	}

	if (prevState && !pushed) {
		release = true;
	}		

	prevState = pushed;
}

void Button::detectAlter(bool physPush) {
	if (physPush) {
		pushStartTime = ((float) clock() / CLOCKS_PER_SEC);
	}

	pushed = physPush;
}

bool Button::simple() {
	if (impulsePushed || continuousPushed || fasteningPushed) {
		impulsePushed = false;
		continuousPushed = false;
		fasteningPushed = false;
		change = true;
	}

	if (change) {
		simplePushed = true;
		change = false;
		return false;
	}

	if (pushed && !simplePushed) {
		simplePushed = true;
		return true;
	}
	else if (pushed && simplePushed) {
		return false;
	}
	else if (!pushed) {
		simplePushed = false;
		return false;
	}

	return false;
}

bool Button::impulse()
{
	if (simplePushed || continuousPushed || fasteningPushed) {
		simplePushed = false;
		continuousPushed = false;
		fasteningPushed = false;
		change = true;
	}

	if (change) {
		if (!pushed) {
			change = false;
		}
		return false;
	}

	if (pushed && !impulsePushed)
	{
		impulsePushed = true;
		impulseChange = false;
		return true;
	}
	else if (pushed && impulsePushed) {
		int time = int((((float)clock() / CLOCKS_PER_SEC) - pushStartTime) / impulseTime);

		if (time % 2 != impulseChange) {
			impulseChange = !impulseChange;
			return true;
		}
		else {
			return false;
		}			
	}
	else if (!pushed) {
		impulsePushed = false;
		return false;
	}

	return false;
}

bool Button::continuous()
{
	if (impulsePushed || simplePushed || fasteningPushed) {
		simplePushed = false;
		impulsePushed = false;
		fasteningPushed = false;
		change = true;
	}

    if (change)	{
		if (!pushed) {
			change = false;
		}
		return false;
	}

	if (pushed) {
		continuousPushed = true;
		return true;
	}
	else {
		continuousPushed = false;
		return false;
	}
	return false;
}

bool Button::fastening() {
	if (impulsePushed || simplePushed || continuousPushed) {
		simplePushed = false;
		impulsePushed = false;
		continuousPushed = false;
		change = true;
	}

	if (change) {
		if (!pushed) {
			change = false;
		}

		return false;
	}

	if (pushed && !fasteningPushed) {
		fasteningPushed = true;
		fasteningTime = 2.0;			// TODO: This should be customized by user!
		fasteningRef = pushStartTime;
		return true;
	}
	else if (pushed && fasteningPushed) {
		int time = int((((float)clock() / CLOCKS_PER_SEC) - fasteningRef) / fasteningTime);

		if (time % 2 == 1) {
			fasteningRef += fasteningTime;

			if (fasteningTime * 0.75 < 0.10) {
				fasteningTime = 0.05f;
			}				
			else {
				fasteningTime *= 0.75;
			}
				
			return true;
		}
		else {
			return false;
		}
	}
	else if (!pushed) {
		fasteningPushed = false;
		return false;
	}

	return false;
}

bool Button::released() {
	return release;
}

bool Button::isPushed() {
	return pushed;
}