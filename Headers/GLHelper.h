#ifndef GLHELPER_H
#define GLHELPER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <SHADER/shader_m.h>

#include "MultiMediaRecording/MultiMedia.h"

class GLHelper {
private:
	//number of earlier taken screenshots and videos
	static unsigned int scr;
	static unsigned int vid;
public:
	static GLFWwindow* window;
	static MultiMedia* multiMedia;

	static Shader* selectShader;
	static Shader* mainShader;
	static Shader* levelShader;
	static Shader* playerShader;

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

	static bool fullScreen;

	static void errorCallback(int error, const char* description);
	static void updateViewPortValues(int, int);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

	static void screenCapture();
	static unsigned int loadTexture(char const* path);
	static unsigned char* getRawCharArrayWithSTBI(char const* path, int* width, int* height, int* nrComponents, int type);
	static void FullscreenSwitch();
	static unsigned int FindScreenShotCount();
	static unsigned int FindVideoCount();
	static std::string generateNewVideoName();
};

#endif // !GLHELPER_H

