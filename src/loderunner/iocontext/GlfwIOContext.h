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
	Button lAlt;

	void saveImage(unsigned char*);
	unsigned int findScreenShotCount();
	std::string generateNewVideoName();
	unsigned int findVideoCount();

	bool fullScreen = false;
	void fullscreenSwitch();

	std::string resourcePath = "./{0}/{1}.json";
protected:
	nlohmann::json jsonConfiguration;
	GLFWwindow* window;
	std::map<std::string, std::string> configMap;

	std::shared_ptr<GameConfiguration> gameConfiguration;

	virtual nlohmann::json readJson(std::string key, std::string folder = "resources");
	virtual void dumpJson(std::string key, nlohmann::json, std::string folder = "resources");
public:
	virtual void initialize() override;
	virtual void terminate() override;

	virtual void processInput() override;
	virtual void initFrame() override;
	virtual void finalizeFrame() override;

	unsigned int loadTexture(char const* path) override;
	void takeScreenShot() override;

	virtual void loadConfig(std::shared_ptr<GameConfiguration>) override;
	virtual void saveConfig(std::string key, std::variant<int, float, std::string> value) override;

	std::array<std::array<char, 28>, 16> loadLevel(std::string, short) override;

	GLFWwindow* getWindow() { return this->window; }

	virtual bool shouldClose() override;

	void handleScreenRecording() override;

	virtual nlohmann::json loadGeneratorLevels() override;
	void saveGeneratorLevels(nlohmann::json) override;
#ifdef VIDEO_RECORDING
	std::shared_ptr<MultiMedia> initializeMultimedia();
#endif
};

#endif // ! GLFWIOCONTEXT_H
