#include "GameStates/Pause.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Enemy.h"

#include "Audio/AudioFile.h"

void Pause::start() {
	
}

void Pause::update(float) {
	if (gameContext->getIOContext()->getEnterButton().simple()) {
		gameContext->getAudio()->getAudioFileByID(7)->playPause();		
		gameContext->getAudio()->getAudioFileByID(14)->playPause();

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

	gamePlay->getPlay()->drawScene();
}

void Pause::end() {

}