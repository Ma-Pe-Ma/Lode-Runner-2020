#include "GameStates/Pause.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Enemy.h"

void Pause::start() {
	
}

void Pause::update(float) {
	if (enter.simple()) {
		Audio::SFX[7].PlayPause();		
		Audio::SFX[14].PlayPause();

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

	Gold::DrawGolds();
	Enemy::DrawPaused();
	gamePlay->play->drawLevel();
	gamePlay->WriteGameTime();
}

void Pause::end() {

}