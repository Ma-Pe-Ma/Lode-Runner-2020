#include "States/Intro.h"
#include "States/StateContext.h"

#include "GLHelper.h"
#include "Enemy.h"
#include "Gold.h"
#include "Drawing.h"
#include "Audio.h"
#include "GameTime.h"

#ifdef ANDROID_VERSION
#include <JNIHelper.h>
#endif

void Intro::start() {
	timer = GameTime::getCurrentFrame();
	Audio::sfx[8].playPause();

	if (stateContext->level[stateContext->playerNr] < 1) {
		stateContext->level[stateContext->playerNr] = 1;
	}
	else {
		if (gameVersion == 1) {
			if(stateContext->level[stateContext->playerNr] > 51) {
				stateContext->level[stateContext->playerNr] = 1;
			}
		}
		else if (stateContext->level[stateContext->playerNr] > 150) {
			stateContext->level[stateContext->playerNr] = 1;
		}
	}

	if (stateContext->menuCursor < 2) {
		stateContext->gamePlay->play->loadLevel(stateContext->level[stateContext->playerNr]);
	}
	/*else {
		//stateContext->gamePlay->play->loadLevel(stateContext->level[stateContext->playerNr]);
	}*/

	std::string number = "00" + std::to_string(stateContext->level[stateContext->playerNr]);

	if (stateContext->level[stateContext->playerNr] > 9) {
		number = '0' + std::to_string(stateContext->level[stateContext->playerNr]);
	}

	if (stateContext->level[stateContext->playerNr] > 99) {
		number = std::to_string(stateContext->level[stateContext->playerNr]);
	}

	levelName = "STAGE " + number;
	playerName = "PLAYER " + std::to_string(stateContext->playerNr + 1);
	lifeLeft = "LEFT " + std::to_string(stateContext->playerLife[stateContext->playerNr]);

	std::string point = std::to_string(stateContext->score[stateContext->playerNr]);
	std::string zeros = "";

	for (int i = 0; i < 8 - point.length(); i++) {
		zeros = zeros + '0';
	}

	scoret = "SCORE    " + zeros + point;

	std::string record = std::to_string(stateContext->highScore);
	zeros = "";

	for (int i = 0; i < 8 - record.length(); i++) {
		zeros = zeros + '0';
	}
	hiscore = "HISCORE  " + zeros + record;

	//save starting level
	std::string line;

#if !defined ANDROID_VERSION && !defined __EMSCRIPTEN__
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

void Intro::update(float currentFrame) {
	Drawing::textWriting(levelName, 8, 12);
	if (gameVersion == 0) {
		Drawing::textWriting(playerName, 12, 6);
	}

	Drawing::textWriting(lifeLeft, 19, 12);
	Drawing::textWriting(scoret, 8, 18);
	Drawing::textWriting(hiscore, 8, 20);

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
}