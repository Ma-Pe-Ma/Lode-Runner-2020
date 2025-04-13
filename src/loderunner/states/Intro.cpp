#include "states/Intro.h"

#include "states/StateContext.h"

#include "iocontext/audio/AudioFile.h"

#include "gameplay/Enemy.h"
#include "gameplay/Gold.h"

void Intro::start() {
	startTimePoint = std::chrono::system_clock::now();
	stateContext->getAudio()->getAudioFileByID(8)->playPause();

	int& level = stateContext->getGameConfiguration()->getLevel()[stateContext->getPlayerNr()];
	stateContext->getGameConfiguration()->validateLevel(level);

	//write player id 
	std::string playerString = stateContext->getGameConfiguration()->getGameVersion() != 1 ? "PLAYER " + std::to_string(stateContext->getPlayerNr() + 1) : "";
	playerName = std::make_shared<Text>(Text(playerString, { 12, 6 }));

	//write level number
	std::string levelNumber = std::to_string(level);
	levelNumber.insert(0, 3 - levelNumber.length(), '0');
	levelName = std::make_shared<Text>(Text("STAGE " + levelNumber, {8,12}));

	auto currentPlayer = stateContext->getPlayerNr();

	//write remaining life number
	auto playerLife = stateContext->getPlayerLife()[currentPlayer];
	lifeLeft = std::make_shared<Text>(Text("LEFT " + std::to_string(playerLife), {19, 12}));

	//write current player score
	auto playerScore = stateContext->getPlayerScore();
	std::string point = std::to_string(playerScore[currentPlayer]);
	point.insert(0, 8 - point.length(), '0');
	scoreText = std::make_shared<Text>(Text("SCORE    " + point, { 8,18 }));

	//write high score
	std::string record = std::to_string(stateContext->getHighScore());
	record.insert(0, 8 - record.length(), '0');
	hiscore = std::make_shared<Text>(Text("HISCORE  " + record, { 8,20 }));

	stateContext->getIOContext()->saveConfig("levelNr", std::to_string(level));

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
		auto& buttonInputs = stateContext->getIOContext()->getButtonInputs();
		
		if (buttonInputs.select.simple()) {
			stateContext->transitionToAtEndOfFrame(stateContext->getSelect());
		}

		if (buttonInputs.enter.simple()) {
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

	if (stateContext->getMenuCursor() < 2) {
		stateContext->getGamePlay()->getGameContext()->loadLevel(stateContext->getCurrentLevel());
	}
}