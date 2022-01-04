#include "GLHelper.h"
#include "IOHandler.h"

#ifdef ANDROID_VERSION

#else
#define	 STB_IMAGE_IMPLEMENTATION
#define  STB_IMAGE_WRITE_IMPLEMENTATION
#include <STB/stb_image.h>
#include <STB/stb_image_write.h>
#endif

#include <thread>

#ifdef VIDEO_RECORDING
MultiMedia* GLHelper::multiMedia;
#endif // VIDEO_RECORDING

Shader* GLHelper::selectShader;
Shader* GLHelper::mainShader;
Shader* GLHelper::levelShader;
Shader* GLHelper::playerShader;

unsigned int GLHelper::selectVBO, GLHelper::selectVAO, GLHelper::selectEBO;
unsigned int GLHelper::levelVBO, GLHelper::levelVAO, GLHelper::levelEBO;
unsigned int GLHelper::playerVBO, GLHelper::playerVAO, GLHelper::playerEBO;
unsigned int GLHelper::mainVBO, GLHelper::mainVAO, GLHelper::mainEBO;
unsigned int GLHelper::cursorVAO, GLHelper::cursorVBO, GLHelper::cursorEBO;

unsigned int GLHelper::menuScreen, GLHelper::characterSet, GLHelper::tileSet, GLHelper::selectScreenT;

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

void GLHelper::updateViewPortValues(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	viewPortWidth = SCR_WIDTH;
	viewPortHeight = SCR_HEIGHT;

	if (viewPortHeight % 2 == 1) {
		viewPortHeight--;
	}		

	if (viewPortWidth % 2 == 1) {
		viewPortWidth--;
	}		

	viewPortX = (width - 15.0f / 9 * height) / 2;
	viewPortY = (height - 9.0f / 15 * width) / 2;

	float screenRatio = ((float)width) / height;

	if (screenRatio >= 15.0f / 9) {
		viewPortY = 0;
		viewPortHeight = height;

		if (viewPortHeight % 2 == 1) {
			viewPortHeight--;
		}			

		viewPortWidth = 15.0f / 9 * height;

		if (viewPortWidth % 2 == 1) {
			viewPortWidth--;
		}			

		viewPortX = (width - viewPortWidth) / 2;
	}
	else {
		viewPortX = 0;
		viewPortWidth = width;

		if (viewPortWidth % 2 == 1) {
			viewPortWidth--;
		}			

		viewPortHeight = 9.0f / 15 * viewPortWidth;

		if (viewPortHeight % 2 == 1) {
			viewPortHeight--;
		}			

		viewPortY = (height - viewPortHeight) / 2;
	}
}

#ifdef ANDROID_VERSION
void GLHelper::UpdateViewPort() {
    ndk_helper::GLContext* context = ndk_helper::GLContext::GetInstance();
    updateViewPortValues(context->GetScreenWidth(), context->GetScreenHeight());

    Helper::log("Width: "+std::to_string(context->GetScreenWidth())+", height: "+std::to_string(context->GetScreenHeight()));

    glViewport(GLHelper::viewPortX, GLHelper::viewPortY, GLHelper::viewPortWidth, GLHelper::viewPortHeight);
}

#else
GLFWwindow* GLHelper::window;
bool GLHelper::fullScreen = false;

void GLHelper::errorCallback(int error, const char* description) {
	std::cout << "\n error: " << error << ", description: " << description;
	std::cout << std::hex << "\n hex: 0x" << error;

	std::cout << std::dec;
}

void GLHelper::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	std::cout << "\n updating viewport: " << width << ", " << height;

	updateViewPortValues(width, height);

#ifdef VIDEO_RECORDING
	if (multiMedia != nullptr) {
		multiMedia->windowResized();
	}
#endif // VIDEO_RECORDING

	glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
}

void GLHelper::window_pos_callback(GLFWwindow* window, int xpos, int ypos) {
	if (!fullScreen) {
		windowPosX = xpos;
		windowPosY = ypos;
	}
}

unsigned char* GLHelper::getRawCharArrayWithSTBI(char const* path, int* width, int* height, int* nrComponents, int type) {
	return stbi_load(path, width, height, nrComponents, type);
}

void GLHelper::fullscreenSwitch() {

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

void GLHelper::saveImage(unsigned char* buffer) {
	std::cout << "\n trying to save image!";
	//find next non-existing screenshot identifier
	unsigned int scr = findScreenShotCount();

	std::string sname = "Screenshots/Screenshot-" + std::to_string(scr) + ".png";

	const char* name = sname.c_str();

	stbi_flip_vertically_on_write(true);

	if (!stbi_write_png(name, SCR_WIDTH, SCR_HEIGHT, 3, buffer, 0)) {
		std::cout << "\nERROR: Could not write screenshot file: " << name;
	}
	else std::cout << "\nScreenshot taken as: " << name;

	delete[] buffer;
}

#endif // !ANDROID_VERSION

//Taking Screenshot
void GLHelper::screenCapture() {
#ifdef ANDROID_VERSION

#else
	unsigned char* buffer = new unsigned char[GLHelper::SCR_WIDTH * GLHelper::SCR_HEIGHT * 3];
	glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	std::thread saveThread(saveImage, buffer);
	saveThread.detach();
#endif
}

unsigned int GLHelper::loadTexture(char const* path) {
#ifdef ANDROID_VERSION
	return ndk_helper::JNIHelper::GetInstance()->LoadTexture(path);
#else
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
		}			
		else if (nrComponents == 4) {
			format = GL_RGBA;
		}			

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
#endif
}

void GLHelper::initialize(std::string mainMenuTextureName) {
	glViewport(GLHelper::viewPortX, GLHelper::viewPortY, GLHelper::viewPortWidth, GLHelper::viewPortHeight);

#ifndef ANDROID_VERSION
	GLFWimage icon;
	int iconNrComp;
	icon.pixels = GLHelper::getRawCharArrayWithSTBI("Texture/Runner.png", &icon.width, &icon.height, &iconNrComp, 4);
	glfwSetWindowIcon(GLHelper::window, 1, &icon);
    selectShader = new Shader("Shaders/select_VS.txt", "Shaders/select_FS.txt");
    mainShader = new Shader("Shaders/main_VS.txt", "Shaders/main_FS.txt");
    levelShader = new Shader("Shaders/level_VS.txt", "Shaders/level_FS.txt");
    playerShader = new Shader("Shaders/player_VS.txt", "Shaders/player_FS.txt");
#else
    std::map<std::string, std::string> param;
	selectShader = new Shader("Shaders/select_VS.txt", "Shaders/select_FS.txt", param);
	mainShader = new Shader("Shaders/main_VS.txt", "Shaders/main_FS.txt", param);
	levelShader = new Shader("Shaders/level_VS.txt", "Shaders/level_FS.txt", param);
	playerShader = new Shader("Shaders/player_VS.txt", "Shaders/player_FS.txt", param);
#endif

	glGenVertexArrays(1, &levelVAO);
	glGenBuffers(1, &levelVBO);
	glGenBuffers(1, &levelEBO);

	glBindVertexArray(levelVAO);

	glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, levelEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Drawing::indices), Drawing::indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenVertexArrays(1, &playerVAO);
	glGenBuffers(1, &playerVBO);
	glGenBuffers(1, &playerEBO);

	glBindVertexArray(playerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, playerVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Drawing::indices), Drawing::indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenVertexArrays(1, &mainVAO);
	glGenBuffers(1, &mainVBO);

	glBindVertexArray(mainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Drawing::main_menu), Drawing::main_menu, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Drawing::indices), Drawing::indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glGenVertexArrays(1, &selectVAO);
	glGenBuffers(1, &selectVBO);
	glGenBuffers(1, &selectEBO);

	glBindVertexArray(selectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, selectVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selectEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Drawing::indices), Drawing::indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glGenVertexArrays(1, &cursorVAO);
	glGenBuffers(1, &cursorVBO);
	glGenBuffers(1, &cursorEBO);

	glBindVertexArray(cursorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
	float cursorMapping[] = { 21.0f / 22, 0, 1, 0, 1, 1.0f / 12, 21.0f / 22, 1.0f / 12 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 8, sizeof(float) * 8, cursorMapping);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cursorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Drawing::indices), Drawing::indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	GLHelper::playerShader->use();
	glActiveTexture(GL_TEXTURE0);
	characterSet = GLHelper::loadTexture("Texture/NES - Lode Runner - Characters.png");
	glBindTexture(GL_TEXTURE_2D, characterSet);
	GLHelper::playerShader->setInt("textureA", 0);

	GLHelper::levelShader->use();
	glActiveTexture(GL_TEXTURE1);
	tileSet = GLHelper::loadTexture("Texture/NES - Lode Runner - Tileset.png");
	glBindTexture(GL_TEXTURE_2D, tileSet);
	GLHelper::levelShader->setInt("textureA", 1);

	const char* mainMenuTextureNameChar = mainMenuTextureName.c_str();

	GLHelper::mainShader->use();
	glActiveTexture(GL_TEXTURE2);
	menuScreen = GLHelper::loadTexture(mainMenuTextureNameChar);
	glBindTexture(GL_TEXTURE_2D, menuScreen);
	GLHelper::mainShader->setInt("textureA", 2);

	GLHelper::selectShader->use();
	glActiveTexture(GL_TEXTURE3);
	selectScreenT = GLHelper::loadTexture("Texture/ABC.png");
	glBindTexture(GL_TEXTURE_2D, selectScreenT);
	GLHelper::selectShader->setInt("textureA", 3);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_DEPTH_TEST);
}

void GLHelper::terminate() {
	glDeleteVertexArrays(1, &levelVAO);
	glDeleteBuffers(1, &levelVBO);
	glDeleteBuffers(1, &levelEBO);

	glDeleteVertexArrays(1, &playerVAO);
	glDeleteBuffers(1, &playerVBO);
	glDeleteBuffers(1, &playerEBO);

	glDeleteVertexArrays(1, &cursorVAO);
	glDeleteBuffers(1, &cursorVBO);
	glDeleteBuffers(1, &cursorEBO);

	glDeleteVertexArrays(1, &mainVAO);
	glDeleteBuffers(1, &mainVBO);
	glDeleteBuffers(1, &mainEBO);

	glDeleteVertexArrays(1, &selectVAO);
	glDeleteBuffers(1, &selectVBO);
	glDeleteBuffers(1, &selectEBO);

	glDeleteTextures(1, &menuScreen);
	glDeleteTextures(1, &characterSet);
	glDeleteTextures(1, &tileSet);
	glDeleteTextures(1, &selectScreenT);

	delete GLHelper::levelShader;
	delete GLHelper::mainShader;
	delete GLHelper::playerShader;
	delete GLHelper::selectShader;
}