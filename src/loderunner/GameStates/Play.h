#ifndef PLAY_H
#define PLAY_H

#include "GameState.h"

#include <memory>
#include "Brick.h"
#include "Trapdoor.h"

#include "Rendering/RenderingManager.h"

#include "GameContext.h"

class Shader;
class Text;

class Play : public GameState {
private:		
	void handleNonControlButtons();
public:
	Play() {}
	void drawScene();
	void start() override;
	void update(float) override;
	void end() override;

	void transitionToDeath();
	void transitionToOutro(short, short, short);

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