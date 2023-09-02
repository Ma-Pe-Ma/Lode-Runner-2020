#include "GameStates/Death.h"
#include "Enemy.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"
#include "Gold.h"

#include "Audio/AudioFile.h"
#include "Player.h"

void Death::start() {	
	auto audio = gameContext->getAudio();

	for (auto id : std::vector<int>{ 3, 7, 17 })
	{
		audio->getAudioFileByID(id)->stopAndRewind();
	}

	audio->getAudioFileByID(3)->playPause();

	startTimePoint = std::chrono::system_clock::now();
}

void Death::update() {
	gamePlay->getPlay()->drawScene();

	float deathLength = gameContext->getAudio()->getAudioFileByID(3)->lengthInSec();
	float ellapsedTime = calculateEllapsedTime();

	if (ellapsedTime < deathLength) {
		gameContext->getPlayer()->setGameTime(calculateEllapsedTime());
		gameContext->handlePlayerDying();
	}
	else {
		auto stateContext = gamePlay->getStateContext();

		if (stateContext->menuCursor < 2) {
			if (--stateContext->playerLife[stateContext->playerNr] != 0) {
				stateContext->playerNr = stateContext->menuCursor == 1 ? 1 - stateContext->playerNr : stateContext->playerNr;
				stateContext->transitionToAtEndOfFrame(stateContext->getIntro());				
			}
			else {
				stateContext->transitionToAtEndOfFrame(stateContext->getGameOver());
			}
		}
		else {
			stateContext->transitionToAtEndOfFrame(stateContext->getGenerator());
		}
	}
}

void Death::end() {

}