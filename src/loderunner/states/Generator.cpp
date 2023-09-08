#include "states/Generator.h"

#include "iocontext/audio/AudioFile.h"
#include "states/StateContext.h"

#include "gameplay/Enemy.h"
#include "gameplay/Gold.h"
#include "gameplay/Player.h"

Generator::Generator() {
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 18; j++) {
			if (j == 0) {
				gen[i][j] = 2;
				texture[i][j] = textureMap.at(2);
			}
			else if (i == 0 || i == 29) {
				gen[i][j] = 2;
				texture[i][j] = textureMap.at(2);
			}
			else {
				gen[i][j] = 0;
				texture[i][j] = textureMap.at(0);
			}

			pos[i][j][0] = i;
			pos[i][j][1] = j;
		}
	}

	geX = 1;
	geY = 16;
}

void Generator::start() {
	stateContext->getRenderingManager()->setGeneratorParameters(&pos[0][0][0], &texture[0][0]);

	getStateContext()->getGamePlay()->getGameContext()->clearContainers();
	startTimePoint = std::chrono::system_clock::now();
}

void Generator::update() {
	auto ioContext = stateContext->getIOContext();
	auto audio = stateContext->getAudio();
	auto itemChangeSoundFile = audio->getAudioFileByID(9);
	auto cursorSoundFile = audio->getAudioFileByID(10);

	if (ioContext->getRightButton().simple()) {
		if (++geX > 28) {
			geX = 28;
		}
		else {
			texture[geX - 1][geY] = textureMap.at(gen[geX - 1][geY]);
			itemChangeSoundFile->stopAndRewind();
			itemChangeSoundFile->playPause();
		}
	}

	if (ioContext->getLeftButton().simple()) {
		if (--geX < 1) {
			geX = 1;
		}
		else {
			texture[geX + 1][geY] = textureMap.at(gen[geX + 1][geY]);
			itemChangeSoundFile->stopAndRewind();
			itemChangeSoundFile->playPause();
		}
	}

	if (ioContext->getUpButton().simple()) {
		if (++geY > 16) {
			geY = 16;
		}
		else {
			texture[geX][geY - 1] = textureMap.at(gen[geX][geY - 1]);
			itemChangeSoundFile->stopAndRewind();
			itemChangeSoundFile->playPause();
		}
	}

	if (ioContext->getDownButton().simple()) {
		if (--geY < 1) {
			geY = 1;
		}
		else {
			texture[geX][geY + 1] = textureMap.at(gen[geX][geY + 1]);
			itemChangeSoundFile->stopAndRewind();
			itemChangeSoundFile->playPause();
		}
	}

	//chaning element upwards
	if (ioContext->getRightDigButton().simple()) {
		if (++gen[geX][geY] > 9) {
			gen[geX][geY] = 0;	
		}

		texture[geX][geY] = textureMap.at(gen[geX][geY]);

		cursorSoundFile->stopAndRewind();
		cursorSoundFile->playPause();
	}

	//chaning element ioContext->downwards
	if (ioContext->getLeftDigButton().simple()) {
		if (--gen[geX][geY] < 0) {
			gen[geX][geY] = 9;
		}

		texture[geX][geY] = textureMap.at(gen[geX][geY]);

		cursorSoundFile->stopAndRewind();
		cursorSoundFile->playPause();
	}

	//yellow cursor block
	if (int(3 * calculateEllapsedTime()) % 2) {
		texture[geX][geY] = 54;
	}
	else {
		texture[geX][geY] = textureMap.at(gen[geX][geY]);
	}
	
	stateContext->getRenderingManager()->renderGenerator();

	if (ioContext->getEnterButton().simple()) {
		texture[geX][geY] = textureMap.at(gen[geX][geY]);

		gameContext->generateLevel(gen);
		stateContext->transitionToAtEndOfFrame(stateContext->getGamePlay());
	}

	if (ioContext->getPauseButton().simple()) {
		stateContext->transitionToAtEndOfFrame(stateContext->getMainMenu());
	}
}

void Generator::end() {

}