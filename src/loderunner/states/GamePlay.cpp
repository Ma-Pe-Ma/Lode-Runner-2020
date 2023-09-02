#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "iocontext/Audio/Audio.h"
#include "iocontext/Audio/AudioFile.h"

#include "gameplay/Enemy.h"
#include "gameplay/Gold.h"

#include "gameplay/GameContext.h"

GamePlay::GamePlay() {
	begin = new Begin();	
	begin->setGamePlay(this);

	play = new Play();
	play->setGamePlay(this);

	pause = new Pause();	
	play->setGamePlay(this);

	death = new Death();
	death->setGamePlay(this);

	currentGameState = begin;
}

void GamePlay::setGameContext(std::shared_ptr<GameContext> gameContext)
{
	this->gameContext = gameContext;
	begin->setGameContext(gameContext);
	play->setGameContext(gameContext);
	pause->setGameContext(gameContext);
	death->setGameContext(gameContext);
}

void GamePlay::transitionTo(GameState* newState, bool start, bool end) {
	if (end) {
		currentGameState->end();
	}

	currentGameState = newState;
	currentGameState->setGamePlay(this);

	if (start) {
		currentGameState->start();
	}
}

void GamePlay::transitionToAtEndOfFrame(GameState* newState, bool start, bool end) {
	transitionableAtEndOfFrame = newState;
}

void GamePlay::start() {
	transitionTo(begin);
}

void GamePlay::update() {
	currentGameState->update();
	checkTransitionAtEndofFrame();
}

void GamePlay::end() {
	auto audio = stateContext->getAudio();
	for (auto id : std::vector<int>{ 4, 7 })
	{
		audio->getAudioFileByID(id)->stopAndRewind();
	}
}

void GamePlay::checkTransitionAtEndofFrame()
{
	if (transitionableAtEndOfFrame != nullptr)
	{
		transitionTo(transitionableAtEndOfFrame);
		transitionableAtEndOfFrame = nullptr;
	}
}