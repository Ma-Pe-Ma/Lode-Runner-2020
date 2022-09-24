#include "States/MainMenu.h"
#include "States/StateContext.h"

#include "Audio.h"
#include "GLHelper.h"

#include "IOHandler.h"

void MainMenu::start() {
	stateContext->menuCursor = 0;
	stateContext->playerLife[0] = 5;
	stateContext->playerLife[1] = 5;

	stateContext->score[0] = 0;
	stateContext->score[1] = 0;

	//stateContext->level[0] = 1;
	//stateContext->level[1] = 1;
	stateContext->playerNr = 0;
}

void MainMenu::update(float currentFrame) {
	//main menu music cyclically playing!
	if (Audio::sfx[5].getPlayStatus() == AudioStatus::stopped) {
		Audio::sfx[5].playPause();
	}

	//Drawing main menu
	if (gameVersion == 1) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		glClearColor(110.0 / 256, 93.0 / 256, 243.0 / 256, 1.0f);
	}

	//drawing background
	glClear(GL_COLOR_BUFFER_BIT);
	GLHelper::mainShader->use();
	GLHelper::mainShader->setInt("mode", 0);
	GLHelper::mainShader->setInt("textureA", 2);
	glBindVertexArray(GLHelper::mainVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//changing gamemode
	if (down.simple()) {
		if (++stateContext->menuCursor == 3) {
			stateContext->menuCursor = 0;
		}
	}

	if (up.simple()) {
		if (--stateContext->menuCursor == -1) {
			stateContext->menuCursor = 2;
		}
	}

	if (gameVersion == 1) {
		stateContext->menuCursor = 0;
	}

	//drawing cursor
	if (gameVersion == 0) {
		glBindVertexArray(GLHelper::cursorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, GLHelper::cursorVBO);
		float cursorY = (1.0f - 2 * stateContext->menuCursor) / 14;
		float cursorLocation[] = { -3.0f / 14, cursorY - 1.0f / 14, -6.0f / 35, cursorY - 1.0f / 14, -6.0f / 35, cursorY, -3.0f / 14, cursorY };
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, cursorLocation);

		GLHelper::mainShader->setInt("mode", 1);
		GLHelper::mainShader->setInt("textureA", 1);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//choosing selected gamemode
	if (enter.simple()) {
		Audio::sfx[5].stopAndRewind();

		switch (stateContext->menuCursor) {
			//single player
			case 0:
			//multiplayer
			case 1:
				stateContext->transitionTo(stateContext->intro);
				break;
			//level generator
			case 2:
				stateContext->transitionTo(stateContext->generator);
				break;
		}
	}
}

void MainMenu::end() {
	Audio::sfx[5].stopAndRewind();
}