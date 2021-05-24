#include "GameStates/GameState.h"

GamePlay* GameState::gamePlay;

void GameState::initialize(GamePlay* gamePlay) {
	GameState::gamePlay = gamePlay;
}

void GameState::SetGamePlay(GamePlay* gamePlay) {
	this->gamePlay = gamePlay;
}