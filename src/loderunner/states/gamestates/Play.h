#ifndef PLAY_H
#define PLAY_H

#include "GameState.h"

#include <memory>

#include "iocontext/rendering/RenderingManager.h"

#include "gameplay/Brick.h"
#include "gameplay/Trapdoor.h"
#include "gameplay/GameContext.h"

class Shader;
class Text;

class Play : public GameState {
private:		
	void handleNonControlButtons();
public:
	Play() {}
	void drawScene();
	void start() override;
	void update() override;
	void end() override;

	void transitionToDeath();
	void transitionToOutro(short, short, std::optional<int>);

	void setGameContext(std::shared_ptr<GameContext> gameContext) override
	{
		GameState::setGameContext(gameContext);
		gameContext->setPlayState(this);
	}

	std::shared_ptr<GameContext> getGameContext()
	{
		return this->gameContext;
	}
};

#endif