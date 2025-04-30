#include "states/MainMenu.h"

#include "states/StateContext.h"

#include "iocontext/audio/AudioContext.h"
#include "iocontext/audio/AudioFile.h"

void MainMenu::start() {
	stateContext->setMenuCursor(0);

	stateContext->getPlayerLife()[0] = 5;
	stateContext->getPlayerLife()[1] = 5;

	stateContext->getPlayerScore()[0] = 0;
	stateContext->getPlayerScore()[1] = 0;

	stateContext->setPlayerNr(0);
	
	stateContext->getShowImGuiWindow() = true;

	setTexts();
}

void MainMenu::update() {
	//main menu music cyclically playing!
	auto mainMenuMusic = stateContext->getAudio()->getAudioFileByID(5);

	if (mainMenuMusic ->getPlayStatus() == AudioStatus::stopped) {
		mainMenuMusic->playPause();
	}

	auto& buttonInputs = stateContext->getIOContext()->getButtonInputs();
	short& menuCursor = stateContext->getMenuCursor();

	//changing gamemode
	if (buttonInputs.down.simple()) {
		menuCursor = ++menuCursor > 2 ? 0 : menuCursor;
	}

	if (buttonInputs.up.simple()) {
		menuCursor = --menuCursor < 0 ? 2 : menuCursor;
	}

	if (stateContext->getGameConfiguration()->getGameVersion() == 1) {
		menuCursor = 0;
	}

	stateContext->getRenderingManager()->renderMainMenu(menuCursor, stateContext->getGameConfiguration()->getCurrentMainTexture());

	//choosing selected gamemode
	if (buttonInputs.enter.simple()) {
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
	stateContext->getShowImGuiWindow() = false;
	stateContext->getAudio()->getAudioFileByID(5)->stopAndRewind();
}

void MainMenu::setTexts() {
	auto textList = stateContext->getGameConfiguration()->getCurrentMainMenuTexts();

	stateContext->getRenderingManager()->setTextList(*textList);
	stateContext->getRenderingManager()->initializeCharacters();
}