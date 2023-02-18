#include "States/Select.h"
#include "States/StateContext.h"

#include <string>
#include "Audio.h"

void Select::start() {
	renderingManager->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;
	textList.push_back(levelText);

	renderingManager->setTextList(textList);
	renderingManager->initializeCharacters();
	updateLevelNr(stateContext->level[stateContext->playerNr]);
}

void Select::update(float currentFrame) {
	int& levelNr = stateContext->level[stateContext->playerNr];

	if (leftButton.simple()) {
		setNewValidLevel(--levelNr);
		updateLevelNr(levelNr);
		Audio::sfx[16].stopAndRewind();
		Audio::sfx[16].playPause();
	}

	if (rightButton.simple()) {
		setNewValidLevel(++levelNr);
		updateLevelNr(levelNr);
		Audio::sfx[16].stopAndRewind();
		Audio::sfx[16].playPause();
	}

	if (up.simple()) {
		setNewValidLevel(levelNr += 10);
		updateLevelNr(levelNr);
		Audio::sfx[16].stopAndRewind();
		Audio::sfx[16].playPause();
	}

	if (down.simple()) {
		setNewValidLevel(levelNr -= 10);
		updateLevelNr(levelNr);
		Audio::sfx[16].stopAndRewind();
		Audio::sfx[16].playPause();
	}

	if (enter.simple()) {
		stateContext->transitionTo(stateContext->intro);
		Audio::sfx[8].stopAndRewind();
		Audio::sfx[8].playPause();
	}

	renderingManager->render();
}

void Select::end() {

}

void Select::setNewValidLevel(int& newLevel)
{
	int maxLevelNumber = gameVersion == 0 ? 150 : 51;
	newLevel = newLevel < 1 ? maxLevelNumber + newLevel : newLevel;
	newLevel = newLevel > maxLevelNumber ? newLevel - maxLevelNumber : newLevel;
}

void Select::updateLevelNr(int levelNr)
{
	std::string levelNumber = std::to_string(levelNr);
	levelNumber.insert(0, 3 - levelNumber.length(), '0');

	levelText->changeContent("STAGE " + levelNumber);
}