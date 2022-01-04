#include "IOHandler.h"
#include <map>

Button up, down, leftButton, rightButton, space, enter, pButton, leftDigButton, rightDigButton, REC, lAlt;
std::string levelFileName = "Level/OriginalLevels.txt";
Vector2D debugPos[2];

bool championship = false;
bool usCover = false;
unsigned int startingLevel = 1;
unsigned int recordingHeight;

#ifdef ANDROID_VERSION
JNIEXPORT void JNICALL
Java_com_mpm_lodeRunner_GameActivity_processInput(JNIEnv* env, jobject thiz, jint buttonID, jboolean pushed) {
    switch ((int) buttonID) {
        case 0:
            up.detectAlter((bool) pushed);
            break;
        case 1:
            rightButton.detectAlter((bool) pushed);
            break;
        case 2:
            down.detectAlter((bool) pushed);
            break;
        case 3:
            leftButton.detectAlter((bool) pushed);
            break;
        case 4:
            leftDigButton.detectAlter((bool) pushed);
            break;
        case 5:
            rightDigButton.detectAlter((bool) pushed);
            break;
        case 6:
            space.detectAlter((bool) pushed);
            break;
        case 7:
            enter.detectAlter((bool) pushed);
            break;
    }
}

JNIEXPORT void JNICALL
Java_com_mpm_lodeRunner_GameActivity_initializeGame( JNIEnv* env, jobject thiz, jboolean champ, jboolean ntscCover, jboolean joystick, jint levelIn, jfloat player, jfloat enemy) {
    championship = (bool) champ;

	if (championship) {
		levelFileName = "Level/ChampionshipLevels.txt";
	}

	usCover = (bool) ntscCover;
    startingLevel = (unsigned int) levelIn;

	Enemy::SetPlayerSpeed((float) player);
	Enemy::SetEnemySpeed((float) enemy);
}

JNIEXPORT void JNICALL
Java_com_mpm_lodeRunner_GameActivity_processJoystick( JNIEnv* env, jobject thiz, jint jangle, jint jstrength) {
    int angle = (int) jangle;
    int strength = (int) jstrength;

    const int threshold = 30;
    const int outerAngle = 20;

    if (strength > threshold) {
        rightButton.detectAlter(angle < 45 + outerAngle || angle > 315 - outerAngle);
        up.detectAlter(45 - outerAngle < angle && angle < 135 + outerAngle);
        leftButton.detectAlter(135 - outerAngle < angle && angle < 225 + outerAngle);
        down.detectAlter(225 - outerAngle < angle && angle < 315 + outerAngle);
    }
    else {
        rightButton.detectAlter(false);
        up.detectAlter(false);
        leftButton.detectAlter(false);
        down.detectAlter(false);
    }
}
#else
void processInput(GLFWwindow* window) {
	GLFWgamepadstate state;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);

#ifndef RELEASE_VERSION
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}	
#endif

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.5) {
		leftButton.detect(1);
	}

	else {
		leftButton.detect(0);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.5) {
		rightButton.detect(1);
	}
	else {
		rightButton.detect(0);
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.5) {
		up.detect(1);
	}
	else {
		up.detect(0);
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.5) {
		down.detect(1);
	}
	else {
		down.detect(0);
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		REC.detect(1);
	}
	else {
		REC.detect(0);
	}
#ifndef  RELEASE_VERSION
	float enemySpeed = 0.415f;
	float speed = GameTime::getSpeed();

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		debugPos[0].x = -enemySpeed * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		debugPos[0].x = enemySpeed * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		debugPos[0].y = enemySpeed * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		debugPos[0].y = -enemySpeed * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		debugPos[1].x = -enemySpeed * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		debugPos[1].x = +enemySpeed * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		debugPos[1].y = +enemySpeed * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		debugPos[1].y = -enemySpeed * speed;
	}
#endif // ! RELEASE_VERSION

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0) {
		rightDigButton.detect(1);
	}		
	else {
		rightDigButton.detect(0);
	}	

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0) {
		leftDigButton.detect(1);
	}		
	else {
		leftDigButton.detect(0);
	}		

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS) {
		enter.detect(1);
	}		
	else {
		enter.detect(0);
	}		

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS) {
		space.detect(1);
	}		
	else {
		space.detect(0);
	}		

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		pButton.detect(1);
	}		
	else {
		pButton.detect(0);
	}		

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		lAlt.detect(1);
	}		
	else {
		lAlt.detect(0);
	}	
}

std::map<std::string, std::string> configMap;

int getIntByKey(std::string key, int defaultValue) {
	std::string value = configMap[key];

	if (value != "") {
		try {
			return std::stoi(value);
		}
		catch (std::invalid_argument e) {
			return defaultValue;
		}
	}

	return defaultValue;
}

float getFloatByKey(std::string key, float defaultValue) {
	std::string value = configMap[key];

	if (value != "") {
		try {
			return std::stof(value);
		}
		catch (std::invalid_argument e) {
			return defaultValue;
		}
	}

	return defaultValue;
}

void loadConfig() {
	//FPS = STREAM_FRAME_RATE;

	std::ifstream config("config.txt", std::fstream::in);
	std::string line;	

	if (config.is_open()) {
		while (getline(config, line)) {
			if (line.length() == 0 || line[0] == '#') {
				continue;
			}

			std::string key = line.substr(0, line.find(' '));
			std::string value = line.substr(line.find(' ') + 1);

			configMap[key] = value;
		}
	}
	else {
		std::cout << "\n Config File Not Found!";
	}

	int resolutionMode = getIntByKey("resolution", 0);

	switch (resolutionMode) {
	case 0:
		GLHelper::SCR_WIDTH = 1500;
		GLHelper::SCR_HEIGHT = 900;
		break;
	case 1:
		GLHelper::SCR_WIDTH = 750;
		GLHelper::SCR_HEIGHT = 450;
		break;

	case 2:
		GLHelper::SCR_WIDTH = 3000;
		GLHelper::SCR_HEIGHT = 1800;
		break;
	case 3:
		GLHelper::SCR_WIDTH = 1750;
		GLHelper::SCR_HEIGHT = 1050;
		break;
	case 4:
		GLHelper::SCR_WIDTH = getIntByKey("width", GLHelper::SCR_WIDTH);
		GLHelper::SCR_HEIGHT = getIntByKey("height", GLHelper::SCR_HEIGHT);
	}

	GLHelper::SCR_WIDTH = GLHelper::SCR_WIDTH > 4096 ? 4096 : GLHelper::SCR_WIDTH;
	GLHelper::SCR_HEIGHT = GLHelper::SCR_HEIGHT > 2160 ? 2160 : GLHelper::SCR_HEIGHT;

	Enemy::setPlayerSpeed(getFloatByKey("playerSpeed", 0.9f));
	Enemy::setEnemySpeed(getFloatByKey("enemySpeed", 0.415f));

	GameTime::setFPS(getIntByKey("FPS", 60));
	int levelSet = getIntByKey("levelset", 0);

	switch (levelSet) {
	case 1:
		levelFileName = "level/ChampionshipLevels.txt";
		championship = true;
		break;
	default:
		levelFileName = "level/OriginalLevels.txt";
		break;
	}

	usCover = getIntByKey("USCOVER", 0);
	startingLevel = getIntByKey("levelNr", 1);
	recordingHeight = getIntByKey("RecordingHeight", recordingHeight);

	leftDigButton.setImpulseTime(0.25);
	rightDigButton.setImpulseTime(0.25);

	GLHelper::updateViewPortValues(GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT);
	findScreenShotCount();
	findVideoCount();
	config.close();
}
#endif // !ANDROID_VERSION

unsigned int scr = 0;
unsigned int vid = 0;

unsigned int findScreenShotCount() {
#ifdef ANDROID_VERSION
	return 0;
#else

	while (true) {
		std::string stringName = "Screenshots/Screenshot-" + std::to_string(scr) + ".png";

		std::ifstream fileStream;
		fileStream.open(stringName);

		if (fileStream.fail()) {
			fileStream.close();
			return scr;
		}
		else {
			scr++;
			fileStream.close();
		}
	}

	return 0;
#endif
}

unsigned int findVideoCount() {
#ifdef ANDROID_VERSION
	return 0;
#else
	while (true) {
		std::string stringName = "GameplayVideos/GameplayVideo-" + std::to_string(vid) + ".mkv";

		std::ifstream fileStream;
		fileStream.open(stringName);

		if (fileStream.fail()) {
			fileStream.close();
			return vid;
		}
		else {
			vid++;
			fileStream.close();
		}
	}
	return 0;
#endif
}

std::string generateNewVideoName() {
	unsigned int vid = findVideoCount();
	return "GameplayVideos/GameplayVideo-" + std::to_string(vid) + ".mkv";
}