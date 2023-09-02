#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "State.h"

#include "iocontext/rendering/RenderingManager.h"

#include <memory>
#include <string>

class GameOver : public State {
private:
	const std::shared_ptr<Text> gameOverText = std::make_shared<Text>(Text("GAME OVER", { 12,10 }));
	std::shared_ptr<Text> playerNameText = nullptr;

	void setupRenderingManager();
public:
	void start() override;
	void update() override;
	void end() override;
};

#endif