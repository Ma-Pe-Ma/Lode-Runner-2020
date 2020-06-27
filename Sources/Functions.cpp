#include "Functions.h"

//from left to right, from down to up
void playerTextureMapping(int index) {
	GLHelper::blokk[8] = ((index % 12) * 1.0) / 12;
	GLHelper::blokk[9] = ((index / 12) * 1.0) / 5;

	GLHelper::blokk[10] = ((index % 12 + 1) * 1.0) / 12;
	GLHelper::blokk[11] = ((index / 12) * 1.0) / 5;

	GLHelper::blokk[12] = ((index % 12 + 1) * 1.0) / 12;
	GLHelper::blokk[13] = ((index / 12 + 1) * 1.0) / 5;

	GLHelper::blokk[14] = ((index % 12) * 1.0) / 12;
	GLHelper::blokk[15] = ((index / 12 + 1) * 1.0) / 5;
}

//from up to down, from left to right
void levelTextureMapping(int index) {
	GLHelper::blokk[8] = ((index / 6) * 1.0) / 11;
	GLHelper::blokk[9] = 5.0 / 6 - ((index % 6) * 1.0) / 6;

	GLHelper::blokk[10] = ((index / 6 + 1) * 1.0) / 11;
	GLHelper::blokk[11] = 5.0 / 6 - ((index % 6) * 1.0) / 6;

	GLHelper::blokk[12] = ((index / 6 + 1) * 1.0) / 11;
	GLHelper::blokk[13] = 1.0 - ((index % 6) * 1.0) / 6;

	GLHelper::blokk[14] = ((index / 6) * 1.0) / 11;
	GLHelper::blokk[15] = 1.0 - ((index % 6) * 1.0) / 6;
}

void abcTextureMapping(char karakter) {
	std::string ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (karakter == '.') {

		GLHelper::blokk[8] = float(0) / 16;
		GLHelper::blokk[9] = (3.0f) / 4;

		GLHelper::blokk[10] = float(1) / 16;
		GLHelper::blokk[11] = (3.0f) / 4;

		GLHelper::blokk[12] = float(1) / 16;
		GLHelper::blokk[13] = (4.0f) / 4;

		GLHelper::blokk[14] = float(0) / 16;
		GLHelper::blokk[15] = (4.0f) / 4;

		return;
	}

	if (karakter == ':') {

		GLHelper::blokk[8] = float(1) / 16;
		GLHelper::blokk[9] = (3.0f) / 4;

		GLHelper::blokk[10] = float(2) / 16;
		GLHelper::blokk[11] = (3.0f) / 4;

		GLHelper::blokk[12] = float(2) / 16;
		GLHelper::blokk[13] = (4.0f) / 4;

		GLHelper::blokk[14] = float(1) / 16;
		GLHelper::blokk[15] = (4.0f) / 4;

		return;
	}

	for (int j = 0; j < 10; j++)
		if (j == (int(karakter) - 48)) {
			GLHelper::blokk[8] = (1.0 * j + 0.01) / 16;
			GLHelper::blokk[9] = (1.0) / 2;

			GLHelper::blokk[10] = (j + 1.0) / 16;
			GLHelper::blokk[11] = (1.0) / 2;

			GLHelper::blokk[12] = (j + 1.0) / 16;
			GLHelper::blokk[13] = (3.0) / 4;

			GLHelper::blokk[14] = (1.0 * j + 0.01) / 16;
			GLHelper::blokk[15] = (3.0) / 4;
		}

	for (int i = 0; i < ABC.length(); i++) {
		if (karakter == ABC[i]) {
			if (i < 15) {
				GLHelper::blokk[8] = (float(1.01 + i)) / 16;
				GLHelper::blokk[9] = (float(1)) / 4;

				GLHelper::blokk[10] = (float(2 + i)) / 16;
				GLHelper::blokk[11] = (float(1)) / 4;

				GLHelper::blokk[12] = (float(2 + i)) / 16;
				GLHelper::blokk[13] = (float(2)) / 4;

				GLHelper::blokk[14] = (float(1.01 + i)) / 16;
				GLHelper::blokk[15] = (float(2)) / 4;
				return;
			}

			if (i > 14) {
				GLHelper::blokk[8] = float(i - 14.99) / 16;
				GLHelper::blokk[9] = (0.0) / 4;

				GLHelper::blokk[10] = float(i - 14) / 16;
				GLHelper::blokk[11] = (0.0) / 4;

				GLHelper::blokk[12] = float(i - 14) / 16;
				GLHelper::blokk[13] = (1.0) / 4;

				GLHelper::blokk[14] = float(i - 14.99) / 16;
				GLHelper::blokk[15] = (1.0) / 4;
				return;
			}
		}
	}
}

void TextWriting(std::string text, float i, float j) {
	for (int k = 0; k < text.length(); k++) {
		if (i == 32) {
			i = 0;
			j++;
		}

		if (text.at(k) != ' ') {

			abcTextureMapping(text.at(k));
			GLHelper::selectShader->use();
			GLHelper::selectShader->setVec2("gPos", i, j);
			GLHelper::selectShader->setInt("textureA", 3);

			glBindVertexArray(GLHelper::selectVAO);
			glBindBuffer(GL_ARRAY_BUFFER, GLHelper::selectVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLHelper::blokk), GLHelper::blokk, GL_STATIC_DRAW);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		i++;
	}
}

void DrawLevel(float x, float y, int ref, float holeTimer) {

	if (holeTimer == -2)
		ref = 24;

	levelTextureMapping(ref);

	GLHelper::levelShader->use();
	glBindVertexArray(GLHelper::levelVAO);
	glBindBuffer(GL_ARRAY_BUFFER, GLHelper::levelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLHelper::blokk), GLHelper::blokk, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, (void*) &blokk[8]);
	GLHelper::levelShader->setVec2("gPos", x, y);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DrawEnemy(float x, float y, int ref, Direction direction, Gold* gold) {
	bool dirToShader = true;

	if (direction == right)
		dirToShader = false;

	bool carryGold = false;

	if (gold != nullptr)
		carryGold = true;

	playerTextureMapping(ref);

	GLHelper::playerShader->use();
	glBindVertexArray(GLHelper::playerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, GLHelper::playerVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLHelper::blokk), GLHelper::blokk, GL_STATIC_DRAW);
	GLHelper::playerShader->setVec2("gPos", x, y);
	GLHelper::playerShader->setBool("direction", dirToShader);
	GLHelper::playerShader->setBool("carryGold", carryGold);
	GLHelper::playerShader->setFloat("ref", GLHelper::blokk[8]);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void processInput(GLFWwindow* window) {
	GLFWgamepadstate state;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	speed = speedFactor * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.5)
		leftButton.detect(1);
	else
		leftButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.5)
		rightButton.detect(1);
	else
		rightButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.5)
		up.detect(1);
	else
		up.detect(0);

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.5)
		down.detect(1);
	else
		down.detect(0);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		REC.detect(1);
	else
		REC.detect(0);

	/* (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		debugPos.x = -enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		debugPos.x =  enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		debugPos.y =  enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		debugPos.y = -enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		enemies[2].dPos.x = -enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		enemies[2].dPos.x = +enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		enemies[2].dPos.y = +enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		enemies[2].dPos.y = -enemySpeed * speed;

	*/

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0)
		rightDigButton.detect(1);
	else
		rightDigButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0)
		leftDigButton.detect(1);
	else
		leftDigButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS)
		enter.detect(1);
	else
		enter.detect(0);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS)
		space.detect(1);
	else
		space.detect(0);

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		pButton.detect(1);
	else
		pButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		lAlt.detect(1);
	else
		lAlt.detect(0);
}

void loadConfig() {
	//FPS = STREAM_FRAME_RATE;

	std::ifstream config("config.txt", std::fstream::in);
	std::string setup;

	if (config.is_open()) {
		while (getline(config, setup)) {
			if (setup[0] == '#')
				continue;

			if (setup.compare(0, 5, "width") == 0)
				GLHelper::SCR_WIDTH = std::stoi(setup.substr(6, 4));

			if (setup.compare(0, 6, "height") == 0)
				GLHelper::SCR_HEIGHT = std::stoi(setup.substr(7, 4));

			if (setup.compare(0, 10, "resolution") == 0) {
				if (std::stoi(setup.substr(11, 1)) == 0) {
					GLHelper::SCR_WIDTH = 1500;
					GLHelper::SCR_HEIGHT = 900;
				}

				if (std::stoi(setup.substr(11, 1)) == 1) {
					GLHelper::SCR_WIDTH = 750;
					GLHelper::SCR_HEIGHT = 450;
				}

				if (std::stoi(setup.substr(11, 1)) == 2) {
					GLHelper::SCR_WIDTH = 3000;
					GLHelper::SCR_HEIGHT = 1800;
				}

				if (std::stoi(setup.substr(11, 1)) == 3) {
					GLHelper::SCR_WIDTH = 1750;
					GLHelper::SCR_HEIGHT = 1050;
				}
			}

			if (setup.compare(0, 11, "playerSpeed") == 0)
				playerSpeed = std::stof(setup.substr(12));

			if (setup.compare(0, 10, "enemySpeed") == 0)
				enemySpeed = std::stof(setup.substr(11));

			if (setup.compare(0, 3, "FPS") == 0)
				FPS = std::stoi(setup.substr(4));

			if (setup.compare(0, 8, "levelSet") == 0) {
				if (std::stoi(setup.substr(9)) == 0)
					levelFileName = "level/lodeRunner.baseLevel.txt";

				if (std::stoi(setup.substr(9)) == 1) {
					levelFileName = "level/lodeRunner.champLevel.txt";
					championship = true;

					if (level[0] > 51) {
						level[0] = 51;
						level[1] = 51;
					}
				}
			}

			if (setup.compare(0, 8, "US COVER") == 0)
				usCover = std::stoi(setup.substr(9));

			if (setup.compare(0, 7, "levelNr") == 0) {
				int levelNr = std::stoi(setup.substr(8));

				if (levelNr < 1) levelNr = 1;
				if (levelNr > 150) levelNr = 150;

				level[0] = levelNr;
				level[1] = levelNr;

				if (championship)
					if (level[0] > 51) {
						level[0] = 51;
						level[1] = 51;
					}
			}

			if (setup.compare(0, 16, "Recording height") == 0)
				recordingHeight = std::stoul(setup.substr(17));
		}

		GLHelper::updateViewPortValues(GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT);
	}
	else
		std::cout << "\n Config File Not Found!";

	leftDigButton.setImpulseTime(0.3);
	rightDigButton.setImpulseTime(0.3);

	GLHelper::FindScreenShotCount();
	GLHelper::FindVideoCount();

	std::cout << "window x : " << GLHelper::SCR_WIDTH;
	std::cout << "window y : " << GLHelper::SCR_HEIGHT;

	config.close();
}