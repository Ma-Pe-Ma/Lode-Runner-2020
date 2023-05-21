#include "GameStates/Death.h"
#include "Enemy.h"
#include "States/GamePlay.h"
#include "GameTime.h"
#include "States/StateContext.h"
#include "Gold.h"

#include "Audio/AudioFile.h"

void Death::start() {	
	gameContext->getAudio()->getAudioFileByID(7)->stopAndRewind();
	gameContext->getAudio()->getAudioFileByID(17)->stopAndRewind();
	gameContext->getAudio()->getAudioFileByID(3)->stopAndRewind();
	gameContext->getAudio()->getAudioFileByID(3)->playPause();
	timer = GameTime::getCurrentFrame();
}

void Death::update(float) {
	gamePlay->getPlay()->drawScene();
	
	float deathLength = gameContext->getAudio()->getAudioFileByID(3)->lengthInSec();

	if (GameTime::getCurrentFrame() - timer < deathLength) {
		gameContext->handlePlayerDying();
	}
	else {
		if (gamePlay->getStateContext()->menuCursor < 2) {
			if (--gamePlay->getStateContext()->playerLife[gamePlay->getStateContext()->playerNr] != 0) {
				gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getIntro());
				gamePlay->getStateContext()->playerNr = gamePlay->getStateContext()->menuCursor == 1 ? 1 - gamePlay->getStateContext()->playerNr : gamePlay->getStateContext()->playerNr;
			}
			else {
				gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getGameOver());
			}
		}
		else {
			gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getGenerator());
		}
	}
}

void Death::end() {

}