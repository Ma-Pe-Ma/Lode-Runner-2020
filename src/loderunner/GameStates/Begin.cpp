#include "GameStates/Begin.h"
#include "States/GamePlay.h"

#include "States/StateContext.h"

void Begin::start() {
	startTime = GameTime::getCurrentFrame();
	Audio::sfx[7].playPause();
	GameTime::reset();
}

void Begin::update(float currentFrame) {
	gamePlay->getPlay()->drawScene();

	if (currentFrame - startTime > 2.0f) {
		gamePlay->transitionToAtEndOfFrame(gamePlay->getPlay());
	}

	//levelselect with space
	if (IOHandler::space.simple()) {
		Audio::sfx[17].stopAndRewind();
		Audio::sfx[4].stopAndRewind();
		Audio::sfx[7].stopAndRewind();

		if (gamePlay->getStateContext()->menuCursor < 2) {
			gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getSelect());
		}
		else {
			gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getGenerator());
		}
	}
}

void Begin::end() {

}