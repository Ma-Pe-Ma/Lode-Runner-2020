#include "states/GameOver.h"

#include "iocontext/audio/AudioFile.h"
#include "states/StateContext.h"

void GameOver::start() {
	playerNameText = std::make_shared<Text>(Text("PLAYER " + std::to_string(stateContext->getPlayerNr() + 1), {12.5, 6.0}));
	startTimePoint = std::chrono::system_clock::now();
	stateContext->getAudio()->getAudioFileByID(6)->playPause();

	setupRenderingManager();
}

void GameOver::setupRenderingManager() {
	stateContext->getRenderingManager()->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;

	textList.push_back(gameOverText);
	textList.push_back(playerNameText);

	stateContext->getRenderingManager()->setTextList(textList);
	stateContext->getRenderingManager()->initializeCharacters();
}

void GameOver::update() {
	stateContext->getRenderingManager()->render();

	float ellapsedTime = calculateEllapsedTime();

	if (ellapsedTime > stateContext->getAudio()->getAudioFileByID(6)->lengthInSec()) {
		if (stateContext->getMenuCursor() == 0) {
			stateContext->transitionToAtEndOfFrame(stateContext->getMainMenu());
		}
		else if (stateContext->getMenuCursor() == 1) {
			if (stateContext->getPlayerLife()[1 - stateContext->getPlayerNr()] != 0) {
				stateContext->getPlayerNr() = 1 - stateContext->getPlayerNr();
				stateContext->transitionToAtEndOfFrame(stateContext->getIntro());
			}
			else {
				stateContext->transitionToAtEndOfFrame(stateContext->getMainMenu());
			}
		}
	}
}

void GameOver::end() {
	stateContext->getAudio()->getAudioFileByID(6)->stopAndRewind();
}