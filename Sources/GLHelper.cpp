#include "GLHelper.h"

#define	 STB_IMAGE_IMPLEMENTATION
#define  STB_IMAGE_WRITE_IMPLEMENTATION
#include <STB/stb_image.h>
#include <STB/stb_image_write.h>

unsigned int GLHelper::scr = 0;
unsigned int GLHelper::vid = 0;

GLFWwindow* GLHelper::window;
MultiMedia* GLHelper::multiMedia;

Shader* GLHelper::selectShader;
Shader* GLHelper::mainShader;
Shader* GLHelper::levelShader;
Shader* GLHelper::playerShader;

unsigned int GLHelper::SCR_WIDTH = 1500;
unsigned int GLHelper::SCR_HEIGHT = 900;

unsigned int GLHelper::former_SCR_WIDTH = 1500;
unsigned int GLHelper::former_SCR_HEIGHT = 900;

unsigned int GLHelper::windowPosX = 30;
unsigned int GLHelper::windowPosY = 30;

unsigned int GLHelper::viewPortWidth = SCR_WIDTH;
unsigned int GLHelper::viewPortHeight = SCR_HEIGHT;

unsigned int GLHelper::viewPortX = 0;
unsigned int GLHelper::viewPortY = 0;

bool GLHelper::fullScreen = false;

void GLHelper::updateViewPortValues(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	viewPortWidth = SCR_WIDTH;
	viewPortHeight = SCR_HEIGHT;

	if (viewPortHeight % 2 == 1)
		viewPortHeight--;

	if (viewPortWidth % 2 == 1)
		viewPortWidth--;

	viewPortX = (width - 15.0f / 9 * height) / 2;
	viewPortY = (height - 9.0f / 15 * width) / 2;

	float screenRatio = ((float)width) / height;

	if (screenRatio >= 15.0f / 9) {

		viewPortY = 0;
		viewPortHeight = height;

		if (viewPortHeight % 2 == 1)
			viewPortHeight--;

		viewPortWidth = 15.0f / 9 * height;

		if (viewPortWidth % 2 == 1)
			viewPortWidth--;

		viewPortX = (width - viewPortWidth) / 2;
	}
	else {
		viewPortX = 0;
		viewPortWidth = width;

		if (viewPortWidth % 2 == 1)
			viewPortWidth--;

		viewPortHeight = 9.0f / 15 * viewPortWidth;

		if (viewPortHeight % 2 == 1)
			viewPortHeight--;

		viewPortY = (height - viewPortHeight) / 2;
	}
}

void GLHelper::errorCallback(int error, const char* description) {
	std::cout << "\n error: " << error << ", description: " << description;
	std::cout << std::hex << "\n hex: 0x" << error;

	std::cout << std::dec;
}

void GLHelper::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	std::cout << "\n updating viewport: " << width << ", " << height;

	updateViewPortValues(width, height);

	if (multiMedia != nullptr) {
		multiMedia->windowResized();
	}

	glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
}

void GLHelper::window_pos_callback(GLFWwindow* window, int xpos, int ypos) {

	if (!fullScreen) {
		windowPosX = xpos;
		windowPosY = ypos;
	}
}

//Taking Screenshot
void GLHelper::screenCapture() {
	unsigned char* buffer = new unsigned char[GLHelper::SCR_WIDTH * GLHelper::SCR_HEIGHT * 3];
	glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	//find next non-existing screenshot identifier
	unsigned int scr = FindScreenShotCount();

	std::string sname = "Screenshots/Screenshot-" + std::to_string(scr) + ".png";

	const char* name = sname.c_str();

	stbi_flip_vertically_on_write(true);

	if (!stbi_write_png(name, SCR_WIDTH, SCR_HEIGHT, 3, buffer, 0)) {
		std::cout << "\nERROR: Could not write screenshot file: " << name;
	}
	else std::cout << "\nScreenshot taken as: " << name;

	delete[] buffer;
}

unsigned int GLHelper::loadTexture(char const* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned char* GLHelper::getRawCharArrayWithSTBI(char const* path, int* width, int* height, int* nrComponents, int type) {
	return stbi_load(path, width, height, nrComponents, type);
}

void GLHelper::FullscreenSwitch() {

	if (!fullScreen) {

		fullScreen = !fullScreen;

		former_SCR_HEIGHT = SCR_HEIGHT;
		former_SCR_WIDTH = SCR_WIDTH;

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	}
	else {
		glfwSetWindowMonitor(window, NULL, 0, 0, former_SCR_WIDTH, former_SCR_HEIGHT, 0);
		glfwSetWindowPos(window, windowPosX, windowPosY);

		fullScreen = !fullScreen;
	}

	glfwMakeContextCurrent(window);
}

unsigned int GLHelper::FindScreenShotCount() {
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
}

unsigned int GLHelper::FindVideoCount() {
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
}

std::string GLHelper::generateNewVideoName() {
	unsigned int vid = FindVideoCount();
	return "GameplayVideos/GameplayVideo-" + std::to_string(vid) + ".mkv";
}