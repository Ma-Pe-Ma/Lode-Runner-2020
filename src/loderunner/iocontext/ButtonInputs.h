#ifndef BUTTONINPUTS_H
#define BUTTONINPUTS_H

#include "Button.h"

struct ButtonInputs {
	Button up, down, left, right, select, enter, leftDig, rightDig, config, pause, screenshot;

#ifdef VIDEO_RECORDING
	Button videoButton;
#endif

#ifndef NDEBUG
	Button d1Left, d1Down, d1Right, d1Up, d2Left, d2Down, d2Right, d2Up;
#endif
};

#endif