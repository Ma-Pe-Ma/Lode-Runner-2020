#ifndef MAINMENU_H
#define MAINMENU_H

#include "State.h"
#include <memory>
#include "Rendering/RenderingManager.h"

class MainMenu : public State {

public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif