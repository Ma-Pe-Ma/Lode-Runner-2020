#include "states/gamestates/GameState.h"

void GameState::setGamePlay(GamePlay* gamePlay) {
	this->gamePlay = gamePlay;
}

void GameState::setGameContext(std::shared_ptr<GameContext> gameContext)
{
	this->gameContext = gameContext;
}

float GameState::calculateEllapsedTime()
{
	std::chrono::duration<float, std::milli> work_time = std::chrono::system_clock::now() - startTimePoint;
	return work_time.count() / 1000;
}