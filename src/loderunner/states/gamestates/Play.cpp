#include "states/gamestates/Play.h"

#include "iocontext/audio/AudioFile.h"

#include "states/GamePlay.h"
#include "states/StateContext.h"

#include "gameplay/Enemy.h"
#include "gameplay/Player.h"
#include "gameplay/Gold.h"

#include <vector>

void Play::start() {
	gameContext->setSessionStartTime();
	auto audio = gameContext->getAudio();

	for (auto id : std::vector<int>{ 1, 4, 7 })
	{
		auto audioFile = audio->getAudioFileByID(id);

		if (audioFile->getPlayStatus() == AudioStatus::playing) {
			audioFile->playPause();
		}
	}
}

void Play::update() {
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
	gameContext->setSessionEndTime();
}

void Play::handleNonControlButtons() {

	auto ioContext = gameContext->getIOContext();

	if (ioContext->getEnterButton().simple() || ioContext->getPauseButton().simple()) {
		auto audio = gameContext->getAudio();
	
		for (auto id : std::vector<int>{ 7,14 })
		{
			audio->getAudioFileByID(id)->playPause();
		}		

		for (auto id : std::vector<int>{ 1, 4, 17 })
		{
			auto audioFile = audio->getAudioFileByID(id);

			if (audioFile->getPlayStatus() == AudioStatus::playing) {
				audioFile->playPause();
			}
		}

		gamePlay->transitionToAtEndOfFrame(gamePlay->getPause());
	}

	//levelselect with space
	if (ioContext->getSpaceButton().simple()) {
		auto audio = gameContext->getAudio();

		for (auto id : std::vector<int>{ 4, 7, 17 })
		{
			audio->getAudioFileByID(id)->stopAndRewind();
		}

		auto stateContext = gamePlay->getStateContext();
		if (stateContext->menuCursor < 2) {
			stateContext->transitionToAtEndOfFrame(stateContext->getSelect());
		}
		else {
			stateContext->transitionToAtEndOfFrame(stateContext->getGenerator());
		}
	}
}

void Play::drawScene() {
	gameContext->getRenderingManager()->render();
}

void Play::transitionToDeath() {
	gamePlay->transitionToAtEndOfFrame(gamePlay->getDeath());
}

void Play::transitionToOutro(short killCounter, short goldNr, short fruitID) {
	auto stateContext = gamePlay->getStateContext();
	stateContext->getOutro()->setScoreParameters(killCounter, goldNr, fruitID);
	stateContext->transitionToAtEndOfFrame(stateContext->getOutro());
}