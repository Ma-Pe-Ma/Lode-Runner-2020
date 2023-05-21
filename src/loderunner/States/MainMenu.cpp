#include "States/MainMenu.h"
#include "States/StateContext.h"

#include "Audio/Audio.h"
#include "Audio/AudioFile.h"

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
	if (stateContext->getAudio()->getAudioFileByID(5)->getPlayStatus() == AudioStatus::stopped) {
		stateContext->getAudio()->getAudioFileByID(5)->playPause();
	}

	int& menuCursor = stateContext->menuCursor;

	//changing gamemode
	if (stateContext->getIOContext()->getDownButton().simple()) {
		menuCursor = ++menuCursor > 2 ? 0 : menuCursor;
	}

	if (stateContext->getIOContext()->getUpButton().simple()) {
		menuCursor = --menuCursor < 0 ? 2 : menuCursor;
	}

	if (stateContext->getGameConfiguration()->getGameVersion() == 1) {
		menuCursor = 0;
	}

	stateContext->getRenderingManager()->drawMainMenu(menuCursor, stateContext->getGameConfiguration()->getGameVersion());

	//choosing selected gamemode
	if (stateContext->getIOContext()->getEnterButton().simple()) {
		stateContext->getAudio()->getAudioFileByID(5)->stopAndRewind();

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