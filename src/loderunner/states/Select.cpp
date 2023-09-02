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

	if (stateContext->getIOContext()->getLeftButton().simple()) {
		changeLevelNumber(--levelNr);
	}

	if (stateContext->getIOContext()->getRightButton().simple()) {
		changeLevelNumber(++levelNr);
	}

	if (stateContext->getIOContext()->getUpButton().simple()) {
		changeLevelNumber(levelNr += 10);
	}

	if (stateContext->getIOContext()->getDownButton().simple()) {
		changeLevelNumber(levelNr -= 10);
	}

	if (stateContext->getIOContext()->getEnterButton().simple()) {
		stateContext->transitionToAtEndOfFrame(stateContext->getIntro());
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