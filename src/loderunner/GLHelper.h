#ifndef GLHELPER_H
#define GLHELPER_H

#ifdef ANDROID_VERSION
#include "../shader_a.h"
#else
#include <Shader.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif // ANDROID_VERSION

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMedia.h"
#endif // VIDEO_RECORDING

class GLHelper {
private:

public:
	
#ifdef VIDEO_RECORDING
	static MultiMedia* multiMedia;
#endif

	static Shader* selectShader;
	static Shader* mainShader;
	static Shader* levelShader;
	static Shader* playerShader;

	static unsigned int selectVBO, selectVAO, selectEBO;
	static unsigned int levelVBO, levelVAO, levelEBO;
	static unsigned int playerVBO, playerVAO, playerEBO;
	static unsigned int mainVBO, mainVAO, mainEBO;
	static unsigned int cursorVAO, cursorVBO, cursorEBO;

	static unsigned int viewPortX;
	static unsigned int viewPortY;
	static unsigned int viewPortWidth;
	static unsigned int viewPortHeight;

	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	static unsigned int former_SCR_WIDTH;
	static unsigned int former_SCR_HEIGHT;

	static unsigned int windowPosX;
	static unsigned int windowPosY;

	#ifdef ANDROID_VERSION
    static void updateViewPort();
    #else
	static bool fullScreen;
	static GLFWwindow* window;

	static void errorCallback(int error, const char* description);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

	static unsigned char* getRawCharArrayWithSTBI(char const* path, int* width, int* height, int* nrComponents, int type);
	static void fullscreenSwitch();
	static void saveImage(unsigned char*);
	#endif

	static void updateViewPortValues(int, int);

	static void screenCapture();
	static unsigned int loadTexture(char const* path);

	static void initialize(std::string);
	static void terminate();

	static unsigned int menuScreen;
	static unsigned int characterSet;
	static unsigned int tileSet;
	static unsigned int selectScreenT;
};

#endif // !GLHELPER_H

