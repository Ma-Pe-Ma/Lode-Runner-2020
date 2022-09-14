#ifndef GLHELPER_H
#define GLHELPER_H

#ifdef ANDROID_VERSION
#include "../shader_a.h"
#else

#include <Shader.h>

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

	extern Shader* selectShader;
	extern Shader* mainShader;
	extern Shader* levelShader;
	extern Shader* playerShader;

	extern unsigned int selectVBO, selectVAO, selectEBO;
	extern unsigned int levelVBO, levelVAO, levelEBO;
	extern unsigned int playerVBO, playerVAO, playerEBO;
	extern unsigned int mainVBO, mainVAO, mainEBO;
	extern unsigned int cursorVAO, cursorVBO, cursorEBO;

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
	extern unsigned int loadTexture(char const* path);

	extern void initialize(std::string);
	extern void terminate();

	extern unsigned int originalMenu;
	extern unsigned int championshipMenu;
	extern unsigned int characterSet;
	extern unsigned int tileSet;
	extern unsigned int selectScreenT;
};

#endif // !GLHELPER_H

