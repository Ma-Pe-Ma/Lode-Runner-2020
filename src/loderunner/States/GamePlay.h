#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "State.h"

#include "GameStates/Begin.h"
#include "GameStates/Play.h"
#include "GameStates/Pause.h"
#include "GameStates/Death.h"

class GameContext;

class GamePlay : public State {
private:
	GameState* currentGameState;

	std::shared_ptr<GameContext> gameContext;

	void transitionTo(GameState*, bool start = true, bool end = true);
	Begin* begin;
	Play* play;
	Pause* pause;
	Death* death;

	GameState* transitionableAtEndOfFrame = nullptr;
	float currentFrame = 0;
public:
	GamePlay();

	void start() override;
	void update(float) override;
	void end() override;

	void transitionToAtEndOfFrame(GameState*, bool start = true, bool end = true);

	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager) override;

	void checkTransitionAtEndofFrame();

	void setGameContext(std::shared_ptr<GameContext> gameContext);

	Begin* getBegin()
	{
		return this->begin;
	}

	void setBegin(Begin* begin)
	{
		this->begin = begin;
	}

	Play* getPlay()
	{
		return this->play;
	}

	void setPlay(Play* play)
	{
		this->play = play;
	}

	Pause* getPause()
	{
		return pause;
	}

	void setPause(Pause* pause)
	{
		this->pause = pause;
	}

	Death* getDeath()
	{
		return this->death;
	}

	void setDeath(Death* death)
	{
		this->death = death;
	}
};

#endif