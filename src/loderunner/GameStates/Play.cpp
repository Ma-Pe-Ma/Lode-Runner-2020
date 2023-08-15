#include "GameStates/Play.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Enemy.h"
#include "Player.h"
#include "Gold.h"

//#include "Generator.h"
#include "GameTime.h"
#include "Audio/AudioFile.h"

void Play::start() {
	//player->SetIdleTime();
	GameTime::setSessionStartTime();

	if (gameContext->getAudio()->getAudioFileByID(4)->getPlayStatus() == AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(4)->playPause();
	}

	if (gameContext->getAudio()->getAudioFileByID(1)->getPlayStatus() == AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(1)->playPause();
	}

	if (gameContext->getAudio()->getAudioFileByID(17)->getPlayStatus() == AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(17)->playPause();
	}
}

void Play::update(float currentFrame) {
	GameTime::update(currentFrame);

	//play gameplay music cyclically
	if (gameContext->getAudio()->getAudioFileByID(7)->getPlayStatus() != AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(7)->playPause();
	}

	//run the game logic
	gameContext->run();

	//draw the game
	drawScene();

	//handle select and pause
	handleNonControlButtons();
}

void Play::end() {
	GameTime::setSessionEndTime();
}

void Play::handleNonControlButtons() {
	if (gameContext->getIOContext()->getEnterButton().simple()) {
		gameContext->getAudio()->getAudioFileByID(7)->playPause();

		if (gameContext->getAudio()->getAudioFileByID(4)->getPlayStatus() == AudioStatus::playing) {
			gameContext->getAudio()->getAudioFileByID(4)->playPause();
		}

		if (gameContext->getAudio()->getAudioFileByID(1)->getPlayStatus() == AudioStatus::playing) {
			gameContext->getAudio()->getAudioFileByID(1)->playPause();
		}

		if (gameContext->getAudio()->getAudioFileByID(17)->getPlayStatus() == AudioStatus::playing) {
			gameContext->getAudio()->getAudioFileByID(17)->playPause();
		}

		gameContext->getAudio()->getAudioFileByID(14)->playPause();

		gamePlay->transitionToAtEndOfFrame(gamePlay->getPause());
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

void Play::drawScene() {
	float gameTime = GameTime::getGameTime();
	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;
	gameContext->getRenderingManager()->setLadderFlashFactor(ladderFactor);
	gameContext->getRenderingManager()->render();
}

void Play::transitionToDeath() {
	gamePlay->transitionToAtEndOfFrame(gamePlay->getDeath());
}

void Play::transitionToOutro(short killCounter, short goldNr, short fruitID) {
	gamePlay->getStateContext()->getOutro()->setScoreParameters(killCounter, goldNr, fruitID);
	gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getOutro());
}