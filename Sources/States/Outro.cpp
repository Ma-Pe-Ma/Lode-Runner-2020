#include "States/Outro.h"
#include "Audio.h"
#include "GameTime.h"
#include "Drawing.h"
#include "States/StateContext.h"

void Outro::setScoreParameters(short killCounter, short goldCounter, short fruitID) {
	enemyScore = killCounter * 200;
	goldScore = goldCounter * 200;
	this->fruitID = fruitID;
}

void Outro::start() {
	Audio::sfx[13].playPause();

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
	Drawing::textWriting(gold_points, 20, 6);
	Drawing::textWriting(enemy_points, 20, 12);
	Drawing::textWriting(total, 14.5, 23.25);

	//draw enemy for score indicator
	Drawing::drawEnemy(11, 9.5, 12, Direction::left, false);

	//draw Gold for score indicator
	int timeFactor = int(2 * currentFrame) % 4;
	timeFactor = timeFactor == 3 ? 1 : timeFactor;
	Drawing::drawLevel(11, 13.25, 36 + timeFactor);

	for (int i = 7; i < 13; i++) {
		for (int k = 0; k < 3; k++) {
			//drawing ladder
			if (i == 9) {
				Drawing::drawLevel(i, k, 12 + timeFactor);
			}
			//drawing bricks
			else if (k == 2) {
				Drawing::drawLevel(i, k, 0);
			}
		}
	}

	//runner climbs ladder
	if (runnerY + GameTime::getSpeed() * 0.1f < 3) {
		runnerY += GameTime::getSpeed() * 0.1;

		int timeFactor = ((currentFrame - timer) * 4);
		int TextureRef = 36 + timeFactor % 4;

		Drawing::drawEnemy(9, runnerY, TextureRef, Direction::left, false);
	}
	//nail bitting after reaching top of ladder
	else {
		int timeFactor = int((currentFrame - timer) * 3) % 4;
		Drawing::drawEnemy(9, 3, 44 + timeFactor, Direction::left, false);
	}

	if (GameTime::getCurrentFrame() - timer > Audio::sfx[13].lengthInSec() || enter.simple()) {
		if (stateContext->menuCursor < 2) {
			stateContext->transitionTo(stateContext->intro);
		}
		else {
			stateContext->transitionTo(stateContext->generator);
		}	
	}	
}

void Outro::end() {
	Audio::sfx[7].stopAndRewind();
	Audio::sfx[13].stopAndRewind();
}