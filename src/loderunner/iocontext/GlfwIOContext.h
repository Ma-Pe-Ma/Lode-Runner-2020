#ifndef GLFWIOCONTEXT_H
#define GLFWIOCONTEXT_H

#include "IOContext.h"

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#else 
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

#include "../Structs/Vector2D.h"

#include <string>
#include <map>

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMedia.h"
#endif

class GlfwIOContext : public IOContext {
	Button pButton, lAlt;
	Vector2D debugPos[2];

	void saveImage(unsigned char*);
	unsigned int findScreenShotCount();
	std::string generateNewVideoName();
	unsigned int findVideoCount();

	bool fullScreen = false;
	void fullscreenSwitch();

#ifdef VIDEO_RECORDING
	std::shared_ptr<MultiMedia> media;
	Button REC;
#endif

protected:
	GLFWwindow* window;
	int getIntByKey(std::string key, int defaultValue);
	float getFloatByKey(std::string key, float defaultValue);
	std::map<std::string, std::string> configMap;

public:
	virtual void processInput() override;

	unsigned int loadTexture(char const* path) override;
	void takeScreenShot() override;
	virtual void loadConfig(std::shared_ptr<GameConfiguration>) override;

	virtual void initialize() override;

	void handleScreenRecording();

	GLFWwindow* getWindow() { return this->window; }

	void loadLevel(std::string, std::function<bool(std::string)>) override;

	virtual void saveConfig(std::string key, std::string value) override;

#ifdef VIDEO_RECORDING
	void initializeMultimedia();
#endif
};

#endif // ! GLFWIOCONTEXT_H
