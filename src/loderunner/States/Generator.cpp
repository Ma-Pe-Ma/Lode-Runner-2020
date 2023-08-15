#include "States/Generator.h"
#include "Audio/AudioFile.h"
#include "Enemy.h"
#include "Gold.h"
#include "Player.h"
#include "States/StateContext.h"

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
}

void Generator::update(float currentFrame) {
	if (stateContext->getIOContext()->getRightButton().simple()) {
		if (++geX > 28) {
			geX = 28;
		}
		else {
			texture[geX - 1][geY] = textureMap.at(gen[geX - 1][geY]);
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	if (stateContext->getIOContext()->getLeftButton().simple()) {
		if (--geX < 1) {
			geX = 1;
		}
		else {
			texture[geX + 1][geY] = textureMap.at(gen[geX + 1][geY]);
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	if (stateContext->getIOContext()->getUpButton().simple()) {
		if (++geY > 16) {
			geY = 16;
		}
		else {
			texture[geX][geY - 1] = textureMap.at(gen[geX][geY - 1]);
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	if (stateContext->getIOContext()->getDownButton().simple()) {
		if (--geY < 1) {
			geY = 1;
		}
		else {
			texture[geX][geY + 1] = textureMap.at(gen[geX][geY + 1]);
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	//chaning element upwards
	if (stateContext->getIOContext()->getRightDigButton().simple()) {
		if (++gen[geX][geY] > 9) {
			gen[geX][geY] = 0;	
		}

		texture[geX][geY] = textureMap.at(gen[geX][geY]);

		stateContext->getAudio()->getAudioFileByID(10)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(10)->playPause();
	}

	//chaning element stateContext->getIOContext()->downwards
	if (stateContext->getIOContext()->getLeftDigButton().simple()) {
		if (--gen[geX][geY] < 0) {
			gen[geX][geY] = 9;
		}

		texture[geX][geY] = textureMap.at(gen[geX][geY]);

		stateContext->getAudio()->getAudioFileByID(10)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(10)->playPause();
	}

	//yellow cursor block
	if (int(3 * currentFrame) % 2) {
		texture[geX][geY] = 54;
	}
	else {
		texture[geX][geY] = textureMap.at(gen[geX][geY]);
	}
	
	stateContext->getRenderingManager()->renderGenerator();

	if (stateContext->getIOContext()->getEnterButton().simple()) {
		texture[geX][geY] = textureMap.at(gen[geX][geY]);

		gameContext->generateLevel(gen);
		stateContext->transitionToAtEndOfFrame(stateContext->getGamePlay());
	}
}

void Generator::end() {

}