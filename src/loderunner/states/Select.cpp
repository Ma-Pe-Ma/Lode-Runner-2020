#include "states/Select.h"
#include "states/StateContext.h"

#include "iocontext/audio/AudioFile.h"

#include <string>
#include <format>

void Select::start() {
	stateContext->getRenderingManager()->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;
	textList.push_back(levelText);

	stateContext->getRenderingManager()->setTextList(textList);
	stateContext->getRenderingManager()->initializeCharacters();
	int& levelNumber = stateContext->getCurrentLevel();

	auto translation = stateContext->getGameConfiguration()->getTranslation();
	auto stageTranslation = translation->getTranslationText("stage");
	std::get<0>(stageTranslation) = std::vformat(std::get<0>(stageTranslation), std::make_format_args(levelNumber));
	levelText->changeContent(std::get<0>(stageTranslation));

	stateContext->getShowImGuiWindow() = true;
}

void Select::update() {
	int& levelNr = stateContext->getCurrentLevel();

	auto& buttonInputs = stateContext->getIOContext()->getButtonInputs();

	if (buttonInputs.left.simple()) {
		changeLevelNumber(--levelNr);
	}

	else if (buttonInputs.right.simple()) {
		changeLevelNumber(++levelNr);
	}

	else if (buttonInputs.up.simple()) {
		changeLevelNumber(levelNr += 10);
	}

	else if (buttonInputs.down.simple()) {
		changeLevelNumber(levelNr -= 10);
	}

	else if (buttonInputs.enter.simple()) {
		stateContext->transitionToAtEndOfFrame(stateContext->getIntro());
	}

	else if (buttonInputs.select.simple()) {
		stateContext->transitionToAtEndOfFrame(stateContext->getMainMenu());
	}
	else {
		changeLevelNumber(levelNr);
	}

	stateContext->getRenderingManager()->render();	
}

void Select::end() {
	stateContext->getShowImGuiWindow() = false;
}

void Select::changeLevelNumber(int& levelNr) {
	if (previousNr != levelNr) {
		stateContext->getGameConfiguration()->validateLevel(levelNr);

		auto translation = stateContext->getGameConfiguration()->getTranslation();
		auto stageTranslation = translation->getTranslationText("stage");
		std::get<0>(stageTranslation) = std::vformat(std::get<0>(stageTranslation), std::make_format_args(levelNr));
		levelText->changeContent(std::get<0>(stageTranslation));

		stateContext->getAudio()->getAudioFileByID(16)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(16)->playPause();

		previousNr = levelNr;
	}	
}