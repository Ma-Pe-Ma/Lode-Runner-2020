#include "states/Select.h"
#include "states/StateContext.h"

#include "iocontext/audio/AudioFile.h"

#include <string>

void Select::start() {
	stateContext->getRenderingManager()->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;
	textList.push_back(levelText);

	stateContext->getRenderingManager()->setTextList(textList);
	stateContext->getRenderingManager()->initializeCharacters();
	levelText->changeContent("STAGE " + std::to_string(stateContext->level[stateContext->playerNr]).insert(0, 3 - std::to_string(stateContext->level[stateContext->playerNr]).length(), '0'));
}

void Select::update() {
	int& levelNr = stateContext->level[stateContext->playerNr];

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

	stateContext->getRenderingManager()->render();
}

void Select::end() {

}

void Select::changeLevelNumber(int& levelNr)
{
	stateContext->getGameConfiguration()->validateLevel(levelNr);
	levelText->changeContent("STAGE " + std::to_string(levelNr).insert(0, 3 - std::to_string(levelNr).length(), '0'));

	stateContext->getAudio()->getAudioFileByID(16)->stopAndRewind();
	stateContext->getAudio()->getAudioFileByID(16)->playPause();
}