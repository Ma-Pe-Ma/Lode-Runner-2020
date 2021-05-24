#include "States/GameOver.h"
#include "GameTime.h"
#include "Audio.h"
#include "Drawing.h"
#include "States/StateContext.h"

void GameOver::start() {
	playerName = "PLAYER " + std::to_string(stateContext->playerNr + 1);
	timer = GameTime::getCurrentFrame();
	Audio::SFX[6].PlayPause();
}

void GameOver::update(float currentFrame) {
	TextWriting(playerName, 12.5, 6);
	TextWriting(gameOverText, 12, 10);

	if (GameTime::getCurrentFrame() - timer > Audio::SFX[6].LengthInSec()) {
		if (stateContext->menuCursor == 0) {
			stateContext->TransitionTo(stateContext->mainMenu);
		}
		else if (stateContext->menuCursor == 1) {
			if (stateContext->playerLife[1 - stateContext->playerNr] != 0) {
				stateContext->playerNr = 1 - stateContext->playerNr;
				stateContext->TransitionTo(stateContext->intro);
			}
			else {
				stateContext->TransitionTo(stateContext->mainMenu);
			}
		}
	}
}

void GameOver::end() {
	Audio::SFX[6].StopAndRewind();
}