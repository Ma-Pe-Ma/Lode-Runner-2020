#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Audio.h"
#include "Enemy.h"
#include "Gold.h"
#include "GameTime.h"

GamePlay::GamePlay() {
	begin = new Begin();
	play = new Play();
	pause = new Pause();
	death = new Death();

	currentGameState = begin;
}

void GamePlay::transitionTo(GameState* newState, bool start, bool end) {
	if (end) {
		currentGameState->end();
	}

	currentGameState = newState;
	//currentGameState->SetGamePlay(this);

	if (start) {
		currentGameState->start();
	}
}

void GamePlay::transitionToAtEndOfFrame(GameState* newState, bool start, bool end) {

}

void GamePlay::start() {
	transitionTo(begin);
}

void GamePlay::update(float currentFrame) {
	currentGameState->update(currentFrame);
}

void GamePlay::end() {
	Audio::sfx[4].stopAndRewind();
	Audio::sfx[7].stopAndRewind();
}