#include "States/MainMenu.h"
#include "States/StateContext.h"

#include "Audio.h"

#include "IOHandler.h"

void MainMenu::start() {
	stateContext->menuCursor = 0;
	stateContext->playerLife[0] = 5;
	stateContext->playerLife[1] = 5;

	stateContext->score[0] = 0;
	stateContext->score[1] = 0;

	//stateContext->level[0] = 1;
	//stateContext->level[1] = 1;

	stateContext->playerNr = 0;
}

void MainMenu::update(float currentFrame) {
	//main menu music cyclically playing!
	if (Audio::sfx[5].getPlayStatus() == AudioStatus::stopped) {
		Audio::sfx[5].playPause();
	}

	int& menuCursor = stateContext->menuCursor;

	//changing gamemode
	if (down.simple()) {
		menuCursor = ++menuCursor > 2 ? 0 : menuCursor;
	}

	if (up.simple()) {
		menuCursor = --menuCursor < 0 ? 2 : menuCursor;
	}

	if (gameVersion == 1) {
		menuCursor = 0;
	}

	renderingManager->drawMainMenu(menuCursor, gameVersion);

	//choosing selected gamemode
	if (enter.simple()) {
		Audio::sfx[5].stopAndRewind();

		switch (menuCursor) {
			//single player
			case 0:
			//multiplayer
			case 1:
				stateContext->transitionTo(stateContext->intro);
				break;
			//level generator
			case 2:
				stateContext->transitionTo(stateContext->generator);
				break;
		}
	}
}

void MainMenu::end() {
	Audio::sfx[5].stopAndRewind();
}