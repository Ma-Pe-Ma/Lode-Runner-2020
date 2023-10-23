#include "states/Intro.h"

#include "states/StateContext.h"

#include "iocontext/audio/AudioFile.h"

#include "gameplay/Enemy.h"
#include "gameplay/Gold.h"

void Intro::start() {
	startTimePoint = std::chrono::system_clock::now();
	stateContext->getAudio()->getAudioFileByID(8)->playPause();

	int& level = stateContext->level[stateContext->playerNr];
	level = level < 1 ? 1 : level;

	int maxLevelNumber = stateContext->getGameConfiguration()->getGameVersion() == 0 ? 150 : 51;
	level = level > maxLevelNumber ? 1 : level;

	/*else {
		//stateContext->gamePlay->play->loadLevel(stateContext->level[stateContext->playerNr]);
	}*/

	//write player id 
	std::string playerString = stateContext->getGameConfiguration()->getGameVersion() == 0 ? "PLAYER " + std::to_string(stateContext->playerNr + 1) : "";
	playerName = std::make_shared<Text>(Text(playerString, { 12, 6 }));

	//write level number
	std::string levelNumber = std::to_string(level);
	levelNumber.insert(0, 3 - levelNumber.length(), '0');
	levelName = std::make_shared<Text>(Text("STAGE " + levelNumber, {8,12}));

	//write remaining life number
	lifeLeft = std::make_shared<Text>(Text("LEFT " + std::to_string(stateContext->playerLife[stateContext->playerNr]), { 19, 12}));

	//write current player score
	std::string point = std::to_string(stateContext->score[stateContext->playerNr]);
	point.insert(0, 8 - point.length(), '0');
	scoreText = std::make_shared<Text>(Text("SCORE    " + point, { 8,18 }));

	//write high score
	std::string record = std::to_string(stateContext->highScore);
	record.insert(0, 8 - record.length(), '0');
	hiscore = std::make_shared<Text>(Text("HISCORE  " + record, { 8,20 }));

	stateContext->getIOContext()->saveConfig("levelNr", std::to_string(stateContext->level[stateContext->playerNr]));

	setupRenderingManager();
}

void Intro::setupRenderingManager()
{
	stateContext->getRenderingManager()->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;

	textList.push_back(levelName);
	textList.push_back(playerName);
	textList.push_back(lifeLeft);
	textList.push_back(scoreText);
	textList.push_back(hiscore);

	stateContext->getRenderingManager()->setTextList(textList);

	stateContext->getRenderingManager()->initializeCharacters();
}

void Intro::update() {
	stateContext->getRenderingManager()->render();

	float ellapsedTime = calculateEllapsedTime();

	if (ellapsedTime < stateContext->getAudio()->getAudioFileByID(8)->lengthInSec()) {
		if (stateContext->getIOContext()->getSelectButton().simple()) {
			stateContext->transitionToAtEndOfFrame(stateContext->getSelect());
		}

		if (stateContext->getIOContext()->getEnterButton().simple()) {
			stateContext->transitionToAtEndOfFrame(stateContext->getGamePlay());
		}
	}
	else {
		stateContext->transitionToAtEndOfFrame(stateContext->getGamePlay());
	}
}

void Intro::end() {
	auto audio = stateContext->getAudio();

	for (auto id : std::vector<int>{ 7, 8 })
	{
		audio->getAudioFileByID(id)->stopAndRewind();
	}

	if (stateContext->menuCursor < 2) {
		stateContext->getGamePlay()->getGameContext()->loadLevel(stateContext->level[stateContext->playerNr]);
	}
}