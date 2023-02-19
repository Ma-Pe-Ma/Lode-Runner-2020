#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "State.h"
#include <string>

#include <Rendering/RenderingManager.h>
#include <memory>

class GameOver : public State {
private:
	const std::shared_ptr<Text> gameOverText = std::make_shared<Text>(Text("GAME OVER", { 12,10 }));
	std::shared_ptr<Text> playerNameText = nullptr;

	void setupRenderingManager();
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif