#ifndef APPCONTAINER_H
#define APPCONTAINER_H

#include <memory>

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include "iocontext/EmscriptenIOContext.h"
#include "iocontext/audio/OpenALAudio.h"
#else 
#include <glad/glad.h>
#include "iocontext/GlfwIOContext.h"		
#include "iocontext/audio/RTAudioContext.h"
#endif

#include "iocontext/GameConfiguration.h"
#include "iocontext/rendering/RenderingManager.h"

#include "states/State.h"
#include "states/StateContext.h"

class AppContainer {
	bool showImguiWindow = true;

	std::shared_ptr<StateContext> stateContext;
	std::shared_ptr<GameContext> gameContext;
	std::shared_ptr<GameConfiguration> gameConfiguration;
	std::shared_ptr<IOContext> ioContext;
	std::shared_ptr<Audio> audio;
	std::shared_ptr<RenderingManager> renderingManager;

	std::vector<std::string> soundNames = {
		"gold", "dig", "dig_prev", "death", "everygold",
		"mainmenu", "gameover", "gameplay", "intro", "step1",
		"step2", "ladder1", "ladder2", "outro", "pause",
		"pole1", "pole2", "fall"
	};

	void handleImGuiConfigurer();
	void finalizeImguiWindow();

public:
	void initialize();
	void update();
	void terminate();

	bool shouldClose();
	unsigned int getFrameRate();
};

#endif