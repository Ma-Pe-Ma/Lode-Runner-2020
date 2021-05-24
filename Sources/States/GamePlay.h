#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "State.h"

#include "GameStates/Begin.h"
#include "GameStates/Play.h"
#include "GameStates/Pause.h"
#include "GameStates/Death.h"

class GamePlay : public State {
private:
	GameState* currentGameState;
public:
	float currentFrame = 0;

	GamePlay();

	void start() override;
	void update(float) override;
	void end() override;

	void TransitionTo(GameState*, bool start = true, bool end = true);
	void TransitionToAtEndOfFrame(GameState*, bool start = true, bool end = true);

	Begin* begin;
	Play* play;
	Pause* pause;
	Death* death;

	float startingTimer = currentFrame;	
	void WriteGameTime();
};

#endif