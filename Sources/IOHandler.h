#ifndef IOHANDLER_H
#define IOHANDLER_H

#ifdef ANDROID_VERSION

#else
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#endif

#include "Button.h"
#include "GLHelper.h"
#include "Structs/Vector2D.h"
#include "Enemy.h"
#include "GameTime.h"

extern Button up, down, leftButton, rightButton, space, enter, pButton, leftDigButton, rightDigButton, REC, lAlt;
extern std::string levelFileName;

extern bool championship;
extern bool usCover;
extern unsigned int startingLevel;
extern unsigned int recordingHeight;

#ifdef ANDROID_VERSION
extern "C" {
JNIEXPORT void JNICALL
Java_com_mpm_lodeRunner_GameActivity_processInput( JNIEnv* env, jobject thiz, jint buttonID, jboolean pushed);

JNIEXPORT void JNICALL
Java_com_mpm_lodeRunner_GameActivity_initializeGame( JNIEnv* env, jobject thiz, jboolean champ, jboolean usCover, jboolean joystick, jint level, jfloat player, jfloat enemy);

JNIEXPORT void JNICALL
Java_com_mpm_lodeRunner_GameActivity_processJoystick( JNIEnv* env, jobject thiz, jint angle, jint strength);
}
#else
void processInput(GLFWwindow*);
void loadConfig();
#endif

//number of earlier taken screenshots and videos
extern unsigned int scr;
extern unsigned int vid;

unsigned int FindScreenShotCount();
unsigned int FindVideoCount();
std::string generateNewVideoName();

extern Vector2D debugPos[2];

#endif // !IOHANDLER_H
