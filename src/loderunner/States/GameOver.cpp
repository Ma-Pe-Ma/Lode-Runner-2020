#include "States/GameOver.h"
#include "GameTime.h"
#include "Audio.h"
#include "States/StateContext.h"

void GameOver::start() {
	playerNameText = std::make_shared<Text>(Text("PLAYER " + std::to_string(stateContext->playerNr + 1), { 12.5, 6.0 }));
	timer = GameTime::getCurrentFrame();
	Audio::sfx[6].playPause();

	setupRenderingManager();
}

void GameOver::setupRenderingManager() {
	renderingManager->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;

	textList.push_back(gameOverText);
	textList.push_back(playerNameText);

	renderingManager->setTextList(textList);
	renderingManager->initializeCharacters();
}

void GameOver::update(float currentFrame) {
	renderingManager->render();

	if (GameTime::getCurrentFrame() - timer > Audio::sfx[6].lengthInSec()) {
		if (stateContext->menuCursor == 0) {
			stateContext->transitionTo(stateContext->mainMenu);
		}
		else if (stateContext->menuCursor == 1) {
			if (stateContext->playerLife[1 - stateContext->playerNr] != 0) {
				stateContext->playerNr = 1 - stateContext->playerNr;
				stateContext->transitionTo(stateContext->intro);
			}
			else {
				stateContext->transitionTo(stateContext->mainMenu);
			}
		}
	}
}

void GameOver::end() {
	Audio::sfx[6].stopAndRewind();
}