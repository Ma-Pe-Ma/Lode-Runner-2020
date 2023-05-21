#include "States/GameOver.h"
#include "GameTime.h"
#include "Audio/AudioFile.h"
#include "States/StateContext.h"

void GameOver::start() {
	playerNameText = std::make_shared<Text>(Text("PLAYER " + std::to_string(stateContext->playerNr + 1), { 12.5, 6.0 }));
	timer = GameTime::getCurrentFrame();
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

void GameOver::update(float currentFrame) {
	stateContext->getRenderingManager()->render();

	if (GameTime::getCurrentFrame() - timer > stateContext->getAudio()->getAudioFileByID(6)->lengthInSec()) {
		if (stateContext->menuCursor == 0) {
			stateContext->transitionToAtEndOfFrame(stateContext->getMainMenu());
		}
		else if (stateContext->menuCursor == 1) {
			if (stateContext->playerLife[1 - stateContext->playerNr] != 0) {
				stateContext->playerNr = 1 - stateContext->playerNr;
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