#include "GameStates/Pause.h"
#include "States/GamePlay.h"
#include "IOHandler.h"
#include "States/StateContext.h"

#include "Enemy.h"

void Pause::start() {
	
}

void Pause::update(float) {
	if (IOHandler::enter.simple()) {
		Audio::sfx[7].playPause();		
		Audio::sfx[14].playPause();

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

	gamePlay->getPlay()->drawScene();
}

void Pause::end() {

}