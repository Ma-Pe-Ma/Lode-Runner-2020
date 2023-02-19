#include "GameStates/GameState.h"

void GameState::setGamePlay(GamePlay* gamePlay) {
	this->gamePlay = gamePlay;
}

void GameState::setGameContext(std::shared_ptr<GameContext> gameContext)
{
	this->gameContext = gameContext;
}