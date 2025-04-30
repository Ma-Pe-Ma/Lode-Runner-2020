#ifndef MAINMENU_H
#define MAINMENU_H

#include "State.h"
#include <memory>
#include "iocontext/rendering/RenderingManager.h"

class MainMenu : public State {

public:
	void start() override;
	void update() override;
	void end() override;

	void setTexts();
};

#endif