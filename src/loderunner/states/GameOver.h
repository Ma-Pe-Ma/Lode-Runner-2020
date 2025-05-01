#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "State.h"

#include "iocontext/rendering/RenderingManager.h"

#include <memory>
#include <string>

class GameOver : public State {
private:
	std::shared_ptr<Text> gameOverText;
	std::shared_ptr<Text> playerNameText;

	void setupRenderingManager();
public:
	void start() override;
	void update() override;
	void end() override;
};

#endif