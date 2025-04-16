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
				texture[i][j] = textureMap.at(2);
			}
			else if (i == 0 || i == 29) {
				texture[i][j] = textureMap.at(2);
			}
			else {
				if (j != 17) {
					gen[j - 1][i - 1] = 0;
				}
				
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
	stateContext->getShowImGuiWindow() = false;

	getStateContext()->getGamePlay()->getGameContext()->clearContainers();
	startTimePoint = std::chrono::system_clock::now();

	generatorGUI.start();
}

void Generator::update() {
	auto ioContext = stateContext->getIOContext();
	auto audio = stateContext->getAudio();
	auto itemChangeSoundFile = audio->getAudioFileByID(9);
	auto cursorSoundFile = audio->getAudioFileByID(10);

	auto& buttonInputs = ioContext->getButtonInputs();

	if (generatorGUI.getGeneratorState() == GeneratorState::edit) {
		if (buttonInputs.right.simple()) {
			if (++geX > 28) {
				geX = 28;
			}
			else {
				texture[geX - 1][geY] = textureMap.at(gen[geY - 1][geX -2]);
				itemChangeSoundFile->stopAndRewind();
				itemChangeSoundFile->playPause();
			}
		}

		if (buttonInputs.left.simple()) {
			if (--geX < 1) {
				geX = 1;
			}
			else {
				texture[geX + 1][geY] = textureMap.at(gen[geY - 1][geX]);
				itemChangeSoundFile->stopAndRewind();
				itemChangeSoundFile->playPause();
			}
		}

		if (buttonInputs.up.simple()) {
			if (++geY > 16) {
				geY = 16;
			}
			else {
				texture[geX][geY - 1] = textureMap.at(gen[geY - 2][geX - 1]);
				itemChangeSoundFile->stopAndRewind();
				itemChangeSoundFile->playPause();
			}
		}

		if (buttonInputs.down.simple()) {
			if (--geY < 1) {
				geY = 1;
			}
			else {
				texture[geX][geY + 1] = textureMap.at(gen[geY][geX - 1]);
				itemChangeSoundFile->stopAndRewind();
				itemChangeSoundFile->playPause();
			}
		}

		//chaning element upwards
		if (buttonInputs.rightDig.simple()) {
			if (++gen[geY - 1][geX - 1] > 9) {
				gen[geY - 1][geX - 1] = 0;
			}

			texture[geX][geY] = textureMap.at(gen[geY - 1][geX - 1]);

			cursorSoundFile->stopAndRewind();
			cursorSoundFile->playPause();
		}

		//chaning element downwards
		if (buttonInputs.leftDig.simple()) {
			if (--gen[geY - 1][geX - 1] < 0) {
				gen[geY - 1][geX - 1] = 9;
			}

			texture[geX][geY] = textureMap.at(gen[geY -1][geX - 1]);

			cursorSoundFile->stopAndRewind();
			cursorSoundFile->playPause();
		}

		//yellow cursor block
		if (int(3 * calculateEllapsedTime()) % 2) {
			texture[geX][geY] = 54;
		}
		else {
			texture[geX][geY] = textureMap.at(gen[geY - 1][geX - 1]);
		}
	}	
	
	stateContext->getRenderingManager()->renderGenerator();

	if (buttonInputs.enter.simple()) {
		texture[geX][geY] = textureMap.at(gen[geY - 1][geX - 1]);

		gameContext->generateLevel(gen);
		stateContext->transitionToAtEndOfFrame(stateContext->getGamePlay());
	}

	if (buttonInputs.select.simple()) {
		stateContext->transitionToAtEndOfFrame(stateContext->getMainMenu());
	}

	generatorGUI.update();
}

void Generator::end() {

}