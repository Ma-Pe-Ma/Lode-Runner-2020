#ifndef MAINMENU_H
#define MAINMENU_H

#include "State.h"
#include <memory>
#include "Rendering/RenderingManager.h"

class MainMenu : public State {

	std::shared_ptr<RenderingManager> renderingManager;

public:
	void start() override;
	void update(float) override;
	void end() override;

	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager)
	{
		this->renderingManager = renderingManager;
	}
};

#endif