#include "GameStates/Death.h"
#include "Enemy.h"
#include "States/GamePlay.h"
#include "GameTime.h"
#include "States/StateContext.h"
#include "Gold.h"

void Death::start() {	
	Audio::sfx[7].stopAndRewind();
	Audio::sfx[17].stopAndRewind();
	Audio::sfx[3].stopAndRewind();
	Audio::sfx[3].playPause();
	timer = GameTime::getCurrentFrame();
}

void Death::update(float) {
	Enemy::drawPlayerDeath();
	gamePlay->play->drawLevel();
	Gold::drawGolds();
	gamePlay->writeGameTime();
	
	float deathLength = Audio::sfx[3].lengthInSec();

	if (GameTime::getCurrentFrame() - timer < deathLength) {
		Enemy::handlePlayerDying();
	}
	else {
		if (gamePlay->stateContext->menuCursor < 2) {
			if (--gamePlay->stateContext->playerLife[gamePlay->stateContext->playerNr] != 0) {
				gamePlay->stateContext->transitionTo(gamePlay->stateContext->intro);
				gamePlay->stateContext->playerNr = gamePlay->stateContext->menuCursor == 1 ? 1 - gamePlay->stateContext->playerNr : gamePlay->stateContext->playerNr;
			}
			else {
				gamePlay->stateContext->transitionTo(gamePlay->stateContext->gameOver);
			}
		}
		else {
			gamePlay->stateContext->transitionTo(gamePlay->stateContext->generator);
		}
	}
}

void Death::end() {

}