#include "GLHelper.h"
#include "IOHandler.h"

#ifdef ANDROID_VERSION

#else
#define	 STB_IMAGE_IMPLEMENTATION
#define  STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#endif


#ifndef __EMSCRIPTEN__
#include <thread>
#endif

#if defined VIDEO_RECORDING
MultiMedia* multiMedia;
#endif // VIDEO_RECORDING

namespace GLHelper {
	Shader* selectShader;
	Shader* mainShader;
	Shader* levelShader;
	Shader* playerShader;

	unsigned int selectVBO, selectVAO, selectEBO;
	unsigned int levelVBO, levelVAO, levelEBO;
	unsigned int playerVBO, playerVAO, playerEBO;
	unsigned int mainVBO, mainVAO, mainEBO;
	unsigned int cursorVAO, cursorVBO, cursorEBO;

	unsigned int originalMenu, championshipMenu, characterSet, tileSet, selectScreenT;

#ifndef EMSCRIPTEN
	unsigned int SCR_WIDTH = 1500;
	unsigned int SCR_HEIGHT = 900;
#else 
	unsigned int SCR_WIDTH = 1100;
	unsigned int SCR_HEIGHT = 660;
#endif

	unsigned int former_SCR_WIDTH = 1500;
	unsigned int former_SCR_HEIGHT = 900;

	unsigned int windowPosX = 30;
	unsigned int windowPosY = 30;

	unsigned int viewPortWidth = SCR_WIDTH;
	unsigned int viewPortHeight = SCR_HEIGHT;

	unsigned int viewPortX = 0;
	unsigned int viewPortY = 0;

	void updateViewPortValues(int width, int height) {
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
	void updateViewPort() {
		ndk_helper::GLContext* context = ndk_helper::GLContext::GetInstance();
		updateViewPortValues(context->GetScreenWidth(), context->GetScreenHeight());

		Helper::log("Width: " + std::to_string(context->GetScreenWidth()) + ", height: " + std::to_string(context->GetScreenHeight()));

		glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
	}

#else
	GLFWwindow* window;
	bool fullScreen = false;

	void errorCallback(int error, const char* description) {
		std::cout << "\n error: " << error << ", description: " << description;
		std::cout << std::hex << "\n hex: 0x" << error;

		std::cout << std::dec;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		//std::cout << "\n updating viewport: " << width << ", " << height;
		updateViewPortValues(width, height);

#ifdef VIDEO_RECORDING
		if (multiMedia != nullptr) {
			multiMedia->windowResized();
		}
#endif // VIDEO_RECORDING

		glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
	}

	void window_pos_callback(GLFWwindow* window, int xpos, int ypos) {
		if (!fullScreen) {
			windowPosX = xpos;
			windowPosY = ypos;
		}
	}

	unsigned char* getRawCharArrayWithSTBI(char const* path, int* width, int* height, int* nrComponents, int type) {
		return stbi_load(path, width, height, nrComponents, type);
	}

	void fullscreenSwitch() {

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

	void saveImage(unsigned char* buffer) {
		std::cout << "\n Trying to save image...";
		//find next non-existing screenshot identifier
		unsigned int scr = findScreenShotCount();

		std::string sname = "Screenshots/Screenshot-" + std::to_string(scr) + ".png";

		const char* name = sname.c_str();

		stbi_flip_vertically_on_write(true);

		if (!stbi_write_png(name, SCR_WIDTH, SCR_HEIGHT, 3, buffer, 0)) {
			std::cout << "\nERROR: Could not write screenshot file: " << name;
		}
		else {
			std::cout << "\nScreenshot taken as: " << name;
		}

		delete[] buffer;
	}

#endif // !ANDROID_VERSION

	//Taking Screenshot
	void screenCapture() {
#if defined ANDROID_VERSION || defined __EMSCRIPTEN__

#else
		unsigned char* buffer = new unsigned char[SCR_WIDTH * SCR_HEIGHT * 3];
		glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);

		std::thread saveThread(saveImage, buffer);
		saveThread.detach();
#endif
	}

	unsigned int loadTexture(char const* path) {
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
#if !defined __EMSCRIPTEN__
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
#endif

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

	void initialize(std::string mainMenuTextureName) {
		glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);

#ifndef ANDROID_VERSION
		GLFWimage icon;
		int iconNrComp;
		icon.pixels = getRawCharArrayWithSTBI("Assets/Texture/Runner.png", &icon.width, &icon.height, &iconNrComp, 4);
		glfwSetWindowIcon(window, 1, &icon);
		selectShader = new Shader("Assets/Shaders/select.vs", "Assets/Shaders/select.fs");
		mainShader = new Shader("Assets/Shaders/main.vs", "Assets/Shaders/main.fs");
		levelShader = new Shader("Assets/Shaders/level.vs", "Assets/Shaders/level.fs");
		playerShader = new Shader("Assets/Shaders/player.vs", "Assets/Shaders/player.fs");
		std::string assetFolder = "Assets/";
#else
		std::string assetFolder = "";
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

		playerShader->use();
		glActiveTexture(GL_TEXTURE0);
		characterSet = loadTexture((assetFolder + "Texture/NES - Lode Runner - Characters.png").c_str());
		glBindTexture(GL_TEXTURE_2D, characterSet);
		playerShader->setInt("textureA", 0);

		levelShader->use();
		glActiveTexture(GL_TEXTURE1);
		tileSet = loadTexture((assetFolder + "Texture/NES - Lode Runner - Tileset.png").c_str());
		glBindTexture(GL_TEXTURE_2D, tileSet);
		levelShader->setInt("textureA", 1);

		const char* mainMenuTextureNameChar = mainMenuTextureName.c_str();

		mainShader->use();
		glActiveTexture(GL_TEXTURE2);
		originalMenu = loadTexture(mainMenuTextureNameChar);
		championshipMenu = loadTexture("Assets/Texture/Championship.png");

		if (gameVersion == 0) {
			glBindTexture(GL_TEXTURE_2D, originalMenu);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, championshipMenu);
		}

		mainShader->setInt("textureA", 2);

		selectShader->use();
		glActiveTexture(GL_TEXTURE3);
		selectScreenT = loadTexture((assetFolder + "Texture/ABC.png").c_str());
		glBindTexture(GL_TEXTURE_2D, selectScreenT);
		selectShader->setInt("textureA", 3);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glEnable(GL_DEPTH_TEST);
	}

	void terminate() {
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

		glDeleteTextures(1, &originalMenu);
		glDeleteTextures(1, &championshipMenu);
		glDeleteTextures(1, &characterSet);
		glDeleteTextures(1, &tileSet);
		glDeleteTextures(1, &selectScreenT);

		delete levelShader;
		delete mainShader;
		delete playerShader;
		delete selectShader;
	}
}