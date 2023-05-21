#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Audio/Audio.h"
#include "Audio/AudioFile.h"

#include "Enemy.h"
#include "Gold.h"
#include "GameTime.h"

#include "GameContext.h"

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

void GamePlay::update(float currentFrame) {
	currentGameState->update(currentFrame);

	checkTransitionAtEndofFrame();
}

void GamePlay::end() {
	stateContext->getAudio()->getAudioFileByID(4)->stopAndRewind();
	stateContext->getAudio()->getAudioFileByID(7)->stopAndRewind();
}

void GamePlay::checkTransitionAtEndofFrame()
{
	if (transitionableAtEndOfFrame != nullptr)
	{
		transitionTo(transitionableAtEndOfFrame);
		transitionableAtEndOfFrame = nullptr;
	}
}