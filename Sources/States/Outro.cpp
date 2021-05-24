#include "States/Outro.h"
#include "Audio.h"
#include "GameTime.h"
#include "Drawing.h"
#include "States/StateContext.h"

void Outro::SetScoreParameters(short killCounter, short goldCounter, short fruitID) {
	enemyScore = killCounter * 200;
	goldScore = goldCounter * 200;
	this->fruitID = fruitID;
}

void Outro::start() {
	Audio::SFX[13].PlayPause();

	if (stateContext->playerLife[stateContext->playerNr] < 9) {
		stateContext->playerLife[stateContext->playerNr]++;
	}

	stateContext->level[stateContext->playerNr]++;
	if (stateContext->level[stateContext->playerNr] > 150) {
		stateContext->level[stateContext->playerNr] = 1;
	}

	stateContext->score[stateContext->playerNr] += enemyScore + goldScore;
	if (stateContext->score[stateContext->playerNr] > stateContext->highScore) {
		stateContext->highScore = stateContext->score[stateContext->playerNr];
	}

	timer = GameTime::getCurrentFrame();
	runnerY = 0;

	gold_points = std::to_string(goldScore) + " POINTS";
	enemy_points = std::to_string(enemyScore) + " POINTS";
	total = "TOTAL " + std::to_string(goldScore + enemyScore) + " POINTS";
}

void Outro::update(float currentFrame) {
	TextWriting(gold_points, 20, 6);
	TextWriting(enemy_points, 20, 12);
	TextWriting(total, 14.5, 23.25);

	//draw enemy for score indicator
	DrawEnemy(11, 9.5, 12, left, false);

	//draw Gold for score indicator
	int timeFactor = int(2 * currentFrame) % 4;
	timeFactor = timeFactor == 3 ? 1 : timeFactor;
	DrawLevel(11, 13.25, 36 + timeFactor);

	for (int i = 7; i < 13; i++) {
		for (int k = 0; k < 3; k++) {
			//drawing ladder
			if (i == 9) {
				DrawLevel(i, k, 12 + timeFactor);
			}
			//drawing bricks
			if (k == 2) {
				DrawLevel(i, k, 0);
			}
		}
	}

	//runner climbs ladder
	if (runnerY + GameTime::getSpeed() * 0.1f < 3) {
		runnerY += GameTime::getSpeed() * 0.1;

		int timeFactor = ((currentFrame - timer) * 4);
		int TextureRef = 36 + timeFactor % 4;

		DrawEnemy(9, runnerY, TextureRef, left, false);
	}
	//nail bitting after reaching top of ladder
	else {
		int timeFactor = int((currentFrame - timer) * 3) % 4;
		DrawEnemy(9, 3, 44 + timeFactor, left, false);
	}

	if (GameTime::getCurrentFrame() - timer > Audio::SFX[13].LengthInSec() || enter.simple()) {
		if (stateContext->menuCursor < 2) {
			stateContext->TransitionTo(stateContext->intro);
		}
		else {
			stateContext->TransitionTo(stateContext->generator);
		}	
	}	
}

void Outro::end() {
	Audio::SFX[7].StopAndRewind();
	Audio::SFX[13].StopAndRewind();
}