#include "States/Intro.h"
#include "States/StateContext.h"

#include "Enemy.h"
#include "Gold.h"
#include "Audio.h"
#include "GameTime.h"

#ifdef ANDROID_VERSION
#include <JNIHelper.h>
#endif

void Intro::start() {
	timer = GameTime::getCurrentFrame();
	Audio::sfx[8].playPause();

	int& level = stateContext->level[stateContext->playerNr];
	level = level < 1 ? 1 : level;

	int maxLevelNumber = gameVersion == 0 ? 150 : 51;
	level = level > maxLevelNumber ? 1 : level;

	/*else {
		//stateContext->gamePlay->play->loadLevel(stateContext->level[stateContext->playerNr]);
	}*/

	//write player id 
	std::string playerString = gameVersion == 0 ? "PLAYER " + std::to_string(stateContext->playerNr + 1) : "";
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

	saveCurrentLevel();

	setupRenderingManager();
}

void Intro::setupRenderingManager()
{
	renderingManager->clearRenderableObjects();

	std::vector<std::shared_ptr<Text>> textList;

	textList.push_back(levelName);
	textList.push_back(playerName);
	textList.push_back(lifeLeft);
	textList.push_back(scoreText);
	textList.push_back(hiscore);

	renderingManager->setTextList(textList);

	renderingManager->initializeCharacters();
}

void Intro::update(float currentFrame) {
	renderingManager->render();

	if (GameTime::getCurrentFrame() - timer < Audio::sfx[8].lengthInSec()) {
		if (space.simple()) {
			stateContext->transitionTo(stateContext->select);
		}

		if (enter.simple()) {
			stateContext->transitionTo(stateContext->gamePlay);
		}
	}
	else {
		stateContext->transitionTo(stateContext->gamePlay);
	}
}

void Intro::end() {
	Audio::sfx[8].stopAndRewind();
	Audio::sfx[7].stopAndRewind();

	if (stateContext->menuCursor < 2) {
		stateContext->gamePlay->play->loadLevel(stateContext->level[stateContext->playerNr]);
	}
}

void Intro::saveCurrentLevel()
{
#if !defined ANDROID_VERSION && !defined __EMSCRIPTEN__
	std::string line;

	std::ifstream configFileOld;
	std::ofstream configFileNew;
	configFileOld.open("config.txt");
	configFileNew.open("config_temp.txt");
	bool levelConfigFound = false;

	while (getline(configFileOld, line)) {
		if (line.length() == 0 || line[0] == '#') {
			configFileNew << line + "\n";
			continue;
		}

		std::string key = line.substr(0, line.find(' '));
		std::string value = line.substr(line.find(' ') + 1);

		if (key == "levelNr") {
			levelConfigFound = true;

			std::string newLine = "levelNr " + std::to_string(stateContext->level[stateContext->playerNr]);
			configFileNew << newLine + "\n";
		}
		else {
			configFileNew << line + "\n";
		}
	}

	if (!levelConfigFound) {
		std::string newLine = "levelNr " + std::to_string(stateContext->level[stateContext->playerNr]);
		configFileNew << newLine + "\n";
	}

	configFileNew.close();
	configFileOld.close();

	remove("config.txt");
	rename("config_temp.txt", "config.txt");
#elif defined ANDROID_VERSION
	ndk_helper::JNIHelper* jniHelper = ndk_helper::JNIHelper::GetInstance();
	jniHelper->setLastLevel(stateContext->level[stateContext->playerNr]);
#endif
}