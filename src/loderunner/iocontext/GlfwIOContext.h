#ifndef GLFWIOCONTEXT_H
#define GLFWIOCONTEXT_H

#include "IOContext.h"

#include <string>
#include <map>

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMedia.h"
#endif

struct GLFWwindow;

class GlfwIOContext : public IOContext {
	Button pButton, lAlt;

	void saveImage(unsigned char*);
	unsigned int findScreenShotCount();
	std::string generateNewVideoName();
	unsigned int findVideoCount();

	bool fullScreen = false;
	void fullscreenSwitch();

protected:
	GLFWwindow* window;
	int getIntByKey(std::string key, int defaultValue);
	float getFloatByKey(std::string key, float defaultValue);
	std::map<std::string, std::string> configMap;

public:
	virtual void initialize() override;
	virtual void terminate() override;

	virtual void processInput() override;
	virtual void finalizeFrame() override;

	unsigned int loadTexture(char const* path) override;
	void takeScreenShot() override;
	virtual void loadConfig(std::shared_ptr<GameConfiguration>) override;
	virtual void saveConfig(std::string key, std::string value) override;

	void loadLevel(std::string, std::function<bool(std::string)>) override;

	GLFWwindow* getWindow() { return this->window; }

	virtual bool shouldClose() override;

	void handleScreenRecording() override;
#ifdef VIDEO_RECORDING
	void initializeMultimedia();
#endif
};

#endif // ! GLFWIOCONTEXT_H
