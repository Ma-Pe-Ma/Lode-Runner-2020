#include "states/GameOver.h"

#include "iocontext/audio/AudioFile.h"
#include "states/StateContext.h"

void GameOver::start() {
	auto translation = stateContext->getGameConfiguration()->getTranslation();
	
	auto gameOverTranslation = translation->getTranslationText("gameOver");
	gameOverText = std::make_shared<Text>(gameOverTranslation);

	auto gameOverPlayerTranslation = translation->getTranslationText("gameOverPlayer");
	int playerNr = stateContext->getPlayerNr() + 1;
	std::get<0>(gameOverPlayerTranslation) = std::vformat(std::get<0>(gameOverPlayerTranslation), std::make_format_args(playerNr));

	playerNameText = std::make_shared<Text>(Text(gameOverPlayerTranslation));
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