#include "States/GameOver.h"
#include "GameTime.h"
#include "Audio.h"
#include "Drawing.h"
#include "States/StateContext.h"

void GameOver::start() {
	playerName = "PLAYER " + std::to_string(stateContext->playerNr + 1);
	timer = GameTime::getCurrentFrame();
	Audio::sfx[6].playPause();
}

void GameOver::update(float currentFrame) {
	Drawing::textWriting(playerName, 12.5, 6);
	Drawing::textWriting(gameOverText, 12, 10);

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