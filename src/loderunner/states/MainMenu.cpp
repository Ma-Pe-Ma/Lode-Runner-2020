#include "states/MainMenu.h"

#include "states/StateContext.h"

#include "iocontext/audio/AudioContext.h"
#include "iocontext/audio/AudioFile.h"

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

void MainMenu::update() {
	//main menu music cyclically playing!
	auto mainMenuMusic = stateContext->getAudio()->getAudioFileByID(5);

	if (mainMenuMusic ->getPlayStatus() == AudioStatus::stopped) {
		mainMenuMusic->playPause();
	}

	auto ioContext = stateContext->getIOContext();
	int& menuCursor = stateContext->menuCursor;

	//changing gamemode
	if (ioContext->getDownButton().simple()) {
		menuCursor = ++menuCursor > 2 ? 0 : menuCursor;
	}

	if (ioContext->getUpButton().simple()) {
		menuCursor = --menuCursor < 0 ? 2 : menuCursor;
	}

	if (stateContext->getGameConfiguration()->getGameVersion() == 1) {
		menuCursor = 0;
	}

	stateContext->getRenderingManager()->renderMainMenu(menuCursor, stateContext->getGameConfiguration()->getGameVersion());

	//choosing selected gamemode
	if (ioContext->getEnterButton().simple()) {
		mainMenuMusic->stopAndRewind();

		switch (menuCursor) {
			//single player
			case 0:
			//multiplayer
			case 1:
				stateContext->transitionToAtEndOfFrame(stateContext->getIntro());
				break;
			//level generator
			case 2:
				stateContext->transitionToAtEndOfFrame(stateContext->getGenerator());
				break;
		}
	}
}

void MainMenu::end() {
	stateContext->getAudio()->getAudioFileByID(5)->stopAndRewind();
}