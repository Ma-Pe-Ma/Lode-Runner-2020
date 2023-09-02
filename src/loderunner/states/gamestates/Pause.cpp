#include "states/gamestates/Pause.h"

#include "iocontext/audio/AudioFile.h"

#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "gameplay/Enemy.h"

void Pause::start() {
	
}

void Pause::update() {
	auto ioContext = gameContext->getIOContext();

	if (ioContext->getEnterButton().simple() || ioContext->getPauseButton().simple()) {
		auto audio = gameContext->getAudio();

		for (auto id : std::vector<int>{ 7, 14 })
		{
			audio->getAudioFileByID(id)->playPause();
		}

		gamePlay->transitionToAtEndOfFrame(gamePlay->getPlay());
	}

	//levelselect with space
	if (ioContext->getSpaceButton().simple()) {
		auto audio = gameContext->getAudio();

		for (auto id : std::vector<int>{ 4, 7, 14 })
		{
			audio->getAudioFileByID(id)->stopAndRewind();
		}

		if (gamePlay->getStateContext()->menuCursor < 2) {
			gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getSelect());
		}
		else {
			gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getGenerator());
		}
	}

	gamePlay->getPlay()->drawScene();
}

void Pause::end() {

}