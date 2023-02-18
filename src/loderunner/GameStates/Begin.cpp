#include "GameStates/Begin.h"
#include "States/GamePlay.h"

#include "States/StateContext.h"

void Begin::start() {
	startTime = GameTime::getCurrentFrame();
	Audio::sfx[7].playPause();
	GameTime::reset();
}

void Begin::update(float currentFrame) {
	gamePlay->play->drawLevel();

	if (currentFrame - startTime > 2.0f) {
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
}

void Begin::end() {

}