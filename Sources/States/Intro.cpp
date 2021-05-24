#include "States/Intro.h"
#include "States/StateContext.h"

#include "GLHelper.h"
#include "Enemy.h"
#include "Gold.h"
#include "Drawing.h"
#include "Audio.h"
#include "GameTime.h"

void Intro::start() {
	timer = GameTime::getCurrentFrame();
	Audio::SFX[8].PlayPause();

	if (stateContext->level[stateContext->playerNr] < 1) {
		stateContext->level[stateContext->playerNr] = 1;
	}
	else {
		if (championship) {
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
}

void Intro::update(float currentFrame) {
	TextWriting(levelName, 8, 12);
	if (!championship) {
		TextWriting(playerName, 12, 6);
	}

	TextWriting(lifeLeft, 19, 12);
	TextWriting(scoret, 8, 18);
	TextWriting(hiscore, 8, 20);

	if (GameTime::getCurrentFrame() - timer < Audio::SFX[8].LengthInSec()) {
		if (space.simple()) {
			stateContext->TransitionTo(stateContext->select);
		}

		if (enter.simple()) {
			stateContext->TransitionTo(stateContext->gamePlay);
		}
	}
	else {
		stateContext->TransitionTo(stateContext->gamePlay);
	}
}

void Intro::end() {
	Audio::SFX[8].StopAndRewind();
	Audio::SFX[7].StopAndRewind();
}