#include "GameStates/Death.h"
#include "Enemy.h"
#include "States/GamePlay.h"
#include "GameTime.h"
#include "States/StateContext.h"
#include "Gold.h"

void Death::start() {	
	//Audio::SFX[7].StopAndRewind();
	Audio::SFX[17].StopAndRewind();
	Audio::SFX[3].StopAndRewind();
	Audio::SFX[3].PlayPause();
	timer = GameTime::getCurrentFrame();
}

void Death::update(float) {
	Enemy::DrawPlayerDeath();
	gamePlay->play->drawLevel();
	Gold::DrawGolds();
	gamePlay->WriteGameTime();
	
	float deathLength = Audio::SFX[3].LengthInSec();

	if (GameTime::getCurrentFrame() - timer < deathLength) {
		Enemy::HandlePlayerDying();
	}
	else {
		if (gamePlay->stateContext->menuCursor < 2) {
			if (--gamePlay->stateContext->playerLife[gamePlay->stateContext->playerNr] != 0) {
				gamePlay->stateContext->TransitionTo(gamePlay->stateContext->intro);
				gamePlay->stateContext->playerNr = gamePlay->stateContext->menuCursor == 1 ? 1 - gamePlay->stateContext->playerNr : gamePlay->stateContext->playerNr;
			}
			else {
				gamePlay->stateContext->TransitionTo(gamePlay->stateContext->gameOver);
			}
		}
		else {
			gamePlay->stateContext->TransitionTo(gamePlay->stateContext->generator);
		}
	}
}

void Death::end() {

}