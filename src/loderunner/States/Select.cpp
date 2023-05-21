#include "States/Select.h"
#include "States/StateContext.h"

#include <string>
#include "Audio/AudioFile.h"

void Select::start() {
	stateContext->getRenderingManager()->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;
	textList.push_back(levelText);

	stateContext->getRenderingManager()->setTextList(textList);
	stateContext->getRenderingManager()->initializeCharacters();
	updateLevelNr(stateContext->level[stateContext->playerNr]);
}

void Select::update(float currentFrame) {
	int& levelNr = stateContext->level[stateContext->playerNr];

	if (stateContext->getIOContext()->getLeftButton().simple()) {
		setNewValidLevel(--levelNr);
		updateLevelNr(levelNr);
		stateContext->getAudio()->getAudioFileByID(16)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(16)->playPause();
	}

	if (stateContext->getIOContext()->getRightButton().simple()) {
		setNewValidLevel(++levelNr);
		updateLevelNr(levelNr);
		stateContext->getAudio()->getAudioFileByID(16)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(16)->playPause();
	}

	if (stateContext->getIOContext()->getUpButton().simple()) {
		setNewValidLevel(levelNr += 10);
		updateLevelNr(levelNr);
		stateContext->getAudio()->getAudioFileByID(16)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(16)->playPause();
	}

	if (stateContext->getIOContext()->getDownButton().simple()) {
		setNewValidLevel(levelNr -= 10);
		updateLevelNr(levelNr);
		stateContext->getAudio()->getAudioFileByID(16)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(16)->playPause();
	}

	if (stateContext->getIOContext()->getEnterButton().simple()) {
		stateContext->transitionToAtEndOfFrame(stateContext->getIntro());
	}

	stateContext->getRenderingManager()->render();
}

void Select::end() {

}

void Select::setNewValidLevel(int& newLevel)
{
	int maxLevelNumber = stateContext->getGameConfiguration()->getGameVersion() ? 150 : 51;
	newLevel = newLevel < 1 ? maxLevelNumber + newLevel : newLevel;
	newLevel = newLevel > maxLevelNumber ? newLevel - maxLevelNumber : newLevel;
}

void Select::updateLevelNr(int levelNr)
{
	std::string levelNumber = std::to_string(levelNr);
	levelNumber.insert(0, 3 - levelNumber.length(), '0');

	levelText->changeContent("STAGE " + levelNumber);
}