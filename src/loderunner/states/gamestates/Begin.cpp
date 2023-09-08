#include "states/gamestates/Begin.h"
#include "states/GamePlay.h"

#include "states/StateContext.h"
#include "iocontext/audio/AudioFile.h"

void Begin::start() {
	startTimePoint = std::chrono::system_clock::now();
	gameContext->getAudio()->getAudioFileByID(7)->playPause();
}

void Begin::update() {
	gamePlay->getPlay()->drawScene();

	float ellapsedTime = calculateEllapsedTime();

	if (ellapsedTime > 2.0f) {
		Play* play = gamePlay->getPlay();
		play->getGameContext()->resetSessionLength();
		gamePlay->transitionToAtEndOfFrame(play);
	}

	//levelselect with space
	else if (gameContext->getIOContext()->getSpaceButton().simple()) {
		auto audio = gameContext->getAudio();		
		for (auto id : std::vector<int>{ 4, 7, 17 })
		{
			audio->getAudioFileByID(id)->stopAndRewind();
		}

		auto stateContext = gamePlay->getStateContext();
		if (stateContext->menuCursor < 2) {
			stateContext->transitionToAtEndOfFrame(stateContext->getSelect());
		}
		else {
			stateContext->transitionToAtEndOfFrame(stateContext->getGenerator());
		}
	}
}

void Begin::end() {

}