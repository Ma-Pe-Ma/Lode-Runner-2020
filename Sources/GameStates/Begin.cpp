#include "GameStates/Begin.h"
#include "States/GamePlay.h"

#include "GameTime.h"
#include "Enemy.h"
#include "../IOHandler.h"
#include "States/StateContext.h"


void Begin::start() {
	startTime = GameTime::getCurrentFrame();
	Audio::SFX[7].PlayPause();
	GameTime::reset();
}

void Begin::update(float currentFrame) {
	Enemy::DrawPaused();
	Gold::DrawGolds();

	gamePlay->play->drawLevel();
	gamePlay->WriteGameTime();

	if (currentFrame - startTime > 2.0f) {
		gamePlay->TransitionTo(gamePlay->play);
	}

	//levelselect with space
	if (space.simple()) {
		Audio::SFX[17].StopAndRewind();
		Audio::SFX[4].StopAndRewind();
		Audio::SFX[7].StopAndRewind();

		if (gamePlay->stateContext->menuCursor < 2) {
			gamePlay->stateContext->TransitionTo(gamePlay->stateContext->select);
		}
		else {
			gamePlay->stateContext->TransitionTo(gamePlay->stateContext->generator);
		}
	}
}

void Begin::end() {

}