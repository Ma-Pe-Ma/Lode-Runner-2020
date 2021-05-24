#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Audio.h"
#include "Enemy.h"
#include "Gold.h"
#include "Drawing.h"
#include "GameTime.h"

GamePlay::GamePlay() {
	begin = new Begin();
	play = new Play();
	pause = new Pause();
	death = new Death();

	currentGameState = begin;
}

void GamePlay::TransitionTo(GameState* newState, bool start, bool end) {
	if (end) {
		currentGameState->end();
	}

	currentGameState = newState;
	//currentGameState->SetGamePlay(this);

	if (start) {
		currentGameState->start();
	}
}

void GamePlay::TransitionToAtEndOfFrame(GameState* newState, bool start, bool end) {

}

void GamePlay::start() {
	TransitionTo(begin);
}

void GamePlay::update(float currentFrame) {
	currentGameState->update(currentFrame);
}

void GamePlay::end() {
	Audio::SFX[4].StopAndRewind();
	Audio::SFX[7].StopAndRewind();
}

void GamePlay::WriteGameTime() {
	std::string timeValue = std::to_string(GameTime::getGameTime());
	timeValue = timeValue.substr(0, timeValue.length() - 5);
	TextWriting("GAMETIME: " + timeValue + " SEC", -5, 0);
}