#include "states/gamestates/Death.h"

#include "states/GamePlay.h"
#include "states/StateContext.h"

#include "iocontext/audio/AudioFile.h"

#include "gameplay/Player.h"

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

		if (stateContext->getMenuCursor() < 2) {
			if (--stateContext->getPlayerLife()[stateContext->getPlayerNr()] != 0) {
				stateContext->getPlayerNr() = stateContext->getMenuCursor() == 1 ? 1 - stateContext->getPlayerNr() : stateContext->getPlayerNr();
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