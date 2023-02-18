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

		viewPortHeight -= viewPortHeight % 2;
		viewPortWidth -= viewPortWidth % 2;

		viewPortX = (width - 15.0f / 9 * height) / 2;
		viewPortY = (height - 9.0f / 15 * width) / 2;

		float screenRatio = ((float) width) / height;

		if (screenRatio >= 15.0f / 9) {
			viewPortY = 0;

			viewPortHeight = height;
			viewPortHeight -= viewPortHeight % 2;

			viewPortWidth = 15.0f / 9 * height;
			viewPortWidth -= viewPortWidth % 2;

			viewPortX = (width - viewPortWidth) / 2;
		}
		else {
			viewPortX = 0;
			
			viewPortWidth = width;
			viewPortWidth -= viewPortWidth % 2;

			viewPortHeight = 9.0f / 15 * viewPortWidth;
			viewPortHeight -= viewPortHeight % 2;

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

	unsigned int loadTexture(char const* path, bool ignoreBlackBackground = false) {
#ifdef ANDROID_VERSION
		return ndk_helper::JNIHelper::GetInstance()->LoadTexture(path);
#else
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

		if (data) {

			if (ignoreBlackBackground && nrComponents == 4)
			{
				int length = width * height * nrComponents;

				for (int i = 0; i < width * height; i++)
				{
					int r = data[4 * i + 0];
					int g = data[4 * i + 1];
					int b = data[4 * i + 2];				

					if (r <= 1 && g <= 1 && b <= 1)
					{
						data[4 * i + 3] = 0;
					}
				}
			}

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
}