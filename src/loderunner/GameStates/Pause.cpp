#include "GameStates/Pause.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Enemy.h"

void Pause::start() {
	
}

void Pause::update(float) {
	if (enter.simple()) {
		Audio::sfx[7].playPause();		
		Audio::sfx[14].playPause();

		gamePlay->transitionTo(gamePlay->play);
	}

	//levelselect with space
	if (space.simple()) {
		Audio::sfx[17].stopAndRewind();
		Audio::sfx[4].stopAndRewind();
		Audio::sfx[7].stopAndRewind();

		if (gamePlay->stateContext->menuCursor < 2) {
			gamePlay->stateContext->transitionTo(gamePlay->stateContext->select);
		}
		else {
			gamePlay->stateContext->transitionTo(gamePlay->stateContext->generator);
		}
	}

	Gold::drawGolds();
	Enemy::drawPaused();
	gamePlay->play->drawLevel();
	gamePlay->writeGameTime();
}

void Pause::end() {

}