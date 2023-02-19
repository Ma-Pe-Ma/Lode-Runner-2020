#ifndef IOHANDLER_H
#define IOHANDLER_H

#ifdef ANDROID_VERSION

#else
#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#else 
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <string>
#endif

#include "Button.h"
#include "Rendering/GLHelper.h"
#include "Structs/Vector2D.h"
#include "Enemy.h"
#include "GameTime.h"

namespace IOHandler  {
	extern Button up, down, leftButton, rightButton, space, enter, pButton, leftDigButton, rightDigButton, REC, lAlt, configButton;
	extern std::string levelFileName;

	extern int gameVersion;
	extern bool usCover;
	extern unsigned int startingLevel;
	extern unsigned int recordingHeight;

	extern float enemySpeed;
	extern float playerSpeed;

#ifdef ANDROID_VERSION
	extern "C" {
		JNIEXPORT void JNICALL
			Java_com_mpm_lodeRunner_GameActivity_processInput(JNIEnv* env, jobject thiz, jint buttonID, jboolean pushed);

		JNIEXPORT void JNICALL
			Java_com_mpm_lodeRunner_GameActivity_initializeGame(JNIEnv* env, jobject thiz, jboolean champ, jboolean usCover, jboolean joystick, jint level, jfloat player, jfloat enemy);

		JNIEXPORT void JNICALL
			Java_com_mpm_lodeRunner_GameActivity_processJoystick(JNIEnv* env, jobject thiz, jint angle, jint strength);
	}
#else
	void processInput(GLFWwindow*);
	void loadConfig();
#endif

	//number of earlier taken screenshots and videos
	extern unsigned int scr;
	extern unsigned int vid;

	unsigned int findScreenShotCount();
	unsigned int findVideoCount();
	std::string generateNewVideoName();

	extern Vector2D debugPos[2];
}

#endif // !IOHANDLER_H
