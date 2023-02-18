#ifndef GLHELPER_H
#define GLHELPER_H

#ifdef ANDROID_VERSION
#include "../shader_a.h"
#else

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#else 
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#endif // ANDROID_VERSION

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMedia.h"
#endif // VIDEO_RECORDING

namespace GLHelper {
	
#ifdef VIDEO_RECORDING
	extern MultiMedia* multiMedia;
#endif
	extern unsigned int viewPortX;
	extern unsigned int viewPortY;
	extern unsigned int viewPortWidth;
	extern unsigned int viewPortHeight;

	extern unsigned int SCR_WIDTH;
	extern unsigned int SCR_HEIGHT;

	extern unsigned int former_SCR_WIDTH;
	extern unsigned int former_SCR_HEIGHT;

	extern unsigned int windowPosX;
	extern unsigned int windowPosY;

#ifdef ANDROID_VERSION
    extern void updateViewPort();
    #else
	extern bool fullScreen;
	extern GLFWwindow* window;

	extern void errorCallback(int error, const char* description);
	extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	extern void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

	extern unsigned char* getRawCharArrayWithSTBI(char const* path, int* width, int* height, int* nrComponents, int type);
	extern void fullscreenSwitch();
	extern void saveImage(unsigned char*);
#endif

	extern void updateViewPortValues(int, int);
	extern void screenCapture();
	extern unsigned int loadTexture(char const* path, bool);
};

#endif // !GLHELPER_H

