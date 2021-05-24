#include "States/Select.h"
#include "States/StateContext.h"

#include <string>
#include "Audio.h"
#include "Drawing.h"

void Select::start() {

}

void Select::update(float currentFrame) {
	if (leftButton.simple()) {
		stateContext->level[stateContext->playerNr]--;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}

	if (rightButton.simple()) {
		stateContext->level[stateContext->playerNr]++;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}

	if (up.simple()) {
		stateContext->level[stateContext->playerNr] += 10;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}

	if (down.simple()) {
		stateContext->level[stateContext->playerNr] -= 10;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}


	if (championship) {
		if (stateContext->level[stateContext->playerNr] < 1) {
			stateContext->level[stateContext->playerNr] = 51 + stateContext->level[stateContext->playerNr];
		}			

		if (stateContext->level[stateContext->playerNr] > 51) {
			stateContext->level[stateContext->playerNr] = stateContext->level[stateContext->playerNr] % 10;
		}			
	}
	else {
		if (stateContext->level[stateContext->playerNr] == 160) {
			stateContext->level[stateContext->playerNr] = 10;
		}

		if (stateContext->level[stateContext->playerNr] > 150) {
			stateContext->level[stateContext->playerNr] = stateContext->level[stateContext->playerNr] % 10;
		}			

		if (stateContext->level[stateContext->playerNr] < 1) {
			stateContext->level[stateContext->playerNr] = 150 + stateContext->level[stateContext->playerNr];
		}			
	}

	if (enter.simple()) {
		stateContext->TransitionTo(stateContext->intro);
		Audio::SFX[8].StopAndRewind();
		Audio::SFX[8].PlayPause();
	}

	std::string levelNumber;

	if (stateContext->level[stateContext->playerNr] > 99) {
		levelNumber = std::to_string(stateContext->level[stateContext->playerNr]);
	}

	if (stateContext->level[stateContext->playerNr] > 9 && stateContext->level[stateContext->playerNr] < 100) {
		levelNumber = '0' + std::to_string(stateContext->level[stateContext->playerNr]);
	}

	if (stateContext->level[stateContext->playerNr] < 10) {
		levelNumber = "00" + std::to_string(stateContext->level[stateContext->playerNr]);
	}

	std::string levelName = "STAGE " + levelNumber;
	TextWriting(levelName, 8, 12);
}

void Select::end() {

}