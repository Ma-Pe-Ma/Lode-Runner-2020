#include "states/Intro.h"

#include "states/StateContext.h"

#include "iocontext/audio/AudioFile.h"

#include "gameplay/Enemy.h"
#include "gameplay/Gold.h"

#include <format>

void Intro::start() {
	startTimePoint = std::chrono::system_clock::now();
	stateContext->getAudio()->getAudioFileByID(8)->playPause();

	int& level = stateContext->getGameConfiguration()->getLevel()[stateContext->getPlayerNr()];
	stateContext->getGameConfiguration()->validateLevel(level);

	auto translation = stateContext->getGameConfiguration()->getTranslation();

	//write player id 
	auto playerIDtranslation = translation->getTranslationText("playerID");
	auto playerNr = stateContext->getPlayerNr() + 1;
	std::get<0>(playerIDtranslation) = std::vformat(std::get<0>(playerIDtranslation), std::make_format_args(playerNr));
	playerName = std::make_shared<Text>(playerIDtranslation);

	//write level number
	auto stageTranslation = translation->getTranslationText("stage");
	std::get<0>(stageTranslation) = std::vformat(std::get<0>(stageTranslation), std::make_format_args(level));
	levelName = std::make_shared<Text>(Text(stageTranslation));

	auto currentPlayer = stateContext->getPlayerNr();

	//write remaining life number
	auto lifeTranslation = translation->getTranslationText("life");
	auto playerLife = stateContext->getPlayerLife()[currentPlayer];
	std::get<0>(lifeTranslation) = std::vformat(std::get<0>(lifeTranslation), std::make_format_args(playerLife));
	lifeLeft = std::make_shared<Text>(Text(lifeTranslation));

	//write current player score
	auto scoreTranslation = translation->getTranslationText("score");
	int playerPoint = stateContext->getPlayerScore()[currentPlayer];
	std::get<0>(scoreTranslation) = std::vformat(std::get<0>(scoreTranslation), std::make_format_args(playerPoint));
	scoreText = std::make_shared<Text>(scoreTranslation);

	//write high score
	auto hiscoreTranslation = translation->getTranslationText("hiscore");
	int hiscorePoint = stateContext->getHighScore();
	std::get<0>(hiscoreTranslation) = std::vformat(std::get<0>(hiscoreTranslation), std::make_format_args(hiscorePoint));
	hiscore = std::make_shared<Text>(Text(hiscoreTranslation));

	stateContext->getIOContext()->saveConfig("levelNr", level);

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