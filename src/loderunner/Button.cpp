#include "Button.h"

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
			pushStartTime = ((float)clock() / CLOCKS_PER_SEC);
		}
	}

	else {
		if (((float)clock() / CLOCKS_PER_SEC) <= pushStartTime + debounceTime)
			pushed = true;
		else
			pushed = false;
	}

	if (prevState && !pushed)
		release = true;

	prevState = pushed;
}

void Button::detectAlter(bool physPush) {
	if (physPush) {
		pushStartTime = ((float)clock() / CLOCKS_PER_SEC);
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
		return 0;
	}

	if (pushed && !simplePushed) {
		simplePushed = true;
		return 1;

	}
	else if (pushed && simplePushed) {
		return 0;

	}
	else if (!pushed) {
		simplePushed = false;
		return 0;
	}

	return 0;
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
		if (!pushed) change = false;
		return 0;
	}

	if (pushed && !impulsePushed)
	{
		impulsePushed = true;
		impulseChange = false;
		return 1;
	}
	else if (pushed && impulsePushed) {
		int time = int((((float)clock() / CLOCKS_PER_SEC) - pushStartTime) / impulseTime);

		if (time % 2 != impulseChange) {
			impulseChange = !impulseChange;
			return 1;
		}
		else
			return 0;
	}
	else if (!pushed) {
		impulsePushed = false;
		return 0;
	}

	return 0;
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
		if (!pushed) change = false;
		return 0;
	}

	if (pushed) {
		continuousPushed = true;
		return 1;
	}
	else {
		continuousPushed = false;
		return 0;
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
		if (!pushed)
			change = false;
		return 0;
	}

	if (pushed && !fasteningPushed) {
		fasteningPushed = true;
		fasteningTime = 2.0;			// TODO: This should be customized by user!
		fasteningRef = pushStartTime;
		return 1;
	}
	else if (pushed && fasteningPushed) {
		int time = int((((float)clock() / CLOCKS_PER_SEC) - fasteningRef) / fasteningTime);

		if (time % 2 == 1) {

			fasteningRef += fasteningTime;

			if (fasteningTime * 0.75 < 0.10)
				fasteningTime = 0.05f;
			else
				fasteningTime *= 0.75;
			return 1;
		}
		else
			return 0;

	}
	else if (!pushed) {
		fasteningPushed = false;
		return 0;
	}

	return 0;
}

bool Button::released() {
	if (release)
		return true;
	return false;
}

bool Button::isPushed() {
	if (pushed)
		return true;

	return false;
}