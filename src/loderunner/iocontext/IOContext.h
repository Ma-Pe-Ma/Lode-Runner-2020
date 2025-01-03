#ifndef IOCONTEXT_H
#define IOCONTEXT_H

#include "ButtonInputs.h"
#include "ScreenParameters.h"

#include <tuple>
#include <memory>
#include <string>
#include <functional>

class GameConfiguration;
class MultiMedia;
class AudioContext;

class IOContext {

protected:
	ButtonInputs buttonInputs;
	ScreenParameters screenParameters;	

#ifdef VIDEO_RECORDING
	std::shared_ptr<MultiMedia> multiMedia;
	std::shared_ptr<AudioContext> audio;
#endif
public:
	virtual void processInput() = 0;
	virtual void initFrame() = 0;
	virtual void finalizeFrame() = 0;

	virtual unsigned int loadTexture(char const* path) = 0;
	virtual void takeScreenShot() = 0;
	virtual void loadConfig(std::shared_ptr<GameConfiguration>) = 0;

	virtual void initialize() = 0;
	virtual void terminate() {}

	virtual void loadLevel(std::string, std::function<bool(std::string)>) = 0;

	virtual void saveConfig(std::string key, std::string value) = 0;

	virtual void handleScreenRecording() {}
	virtual bool shouldClose() = 0;

	ButtonInputs& getButtonInputs() { return this->buttonInputs; }
	ScreenParameters& getScreenParameters() { return this->screenParameters; }

#ifdef VIDEO_RECORDING
	void setAudioContext(std::shared_ptr<AudioContext> audio) { this->audio = audio; }
#endif
};

#endif