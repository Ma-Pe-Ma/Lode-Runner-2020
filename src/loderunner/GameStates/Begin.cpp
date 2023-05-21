#include "GameStates/Begin.h"
#include "States/GamePlay.h"

#include "States/StateContext.h"

#include "../GameTime.h"

#include "Audio/AudioFile.h"

void Begin::start() {
	startTime = GameTime::getCurrentFrame();
	gameContext->getAudio()->getAudioFileByID(7)->playPause();
	GameTime::reset();
}

void Begin::update(float currentFrame) {
	gamePlay->getPlay()->drawScene();

	if (currentFrame - startTime > 2.0f) {
		gamePlay->transitionToAtEndOfFrame(gamePlay->getPlay());
	}

	//levelselect with space
	if (gameContext->getIOContext()->getSpaceButton().simple()) {
		gameContext->getAudio()->getAudioFileByID(17)->stopAndRewind();
		gameContext->getAudio()->getAudioFileByID(4)->stopAndRewind();
		gameContext->getAudio()->getAudioFileByID(7)->stopAndRewind();

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