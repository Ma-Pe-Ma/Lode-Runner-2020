#ifndef EMSCRIPTENIOCONTEXT_H
#define EMSCRIPTENIOCONTEXT_H

#include "GlfwIOContext.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <regex>
#include <optional>

#include "GameConfiguration.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#define TOUCH_BUTTON_NR 9

namespace EmscriptenHandler {
	extern "C" {
		int canvas_get_width();
		int canvas_get_height();

		int screen_get_width();
		int screen_get_height();

		int window_get_width();
		int window_get_height();

		bool is_mobile();

		char* get_cookies();
		void set_cookie(const char*, const char*);

		int getVertexMaxUniformNumber();
		int getFragmentMaxUniformNumber();
	}
}

class EmscriptenRenderingManager;

class EmscriptenIOContext : public GlfwIOContext {
private:
	//maps touch to button
	std::map<int, Button*> buttonMap;
	//maps button to its current state
	std::map<Button*, bool> buttonStateMap{
		{&select, false},
		{&enter, false},
		{&leftDigButton, false},
		{&rightDigButton, false},
		{&leftButton, false},
		{&rightButton, false},
		{&up, false},
		{&down, false},
		//{&configButton, false},
	};
	std::map <Button*, std::tuple<int, int, int, int>> sizedPositionMap;
	//TODO: replace this with optional
	int analogID = -1;

	int analogCenterX = 0;
	int analogCenterY = 0;
	float analogRadius = 0;

	std::tuple<Button*, Button*> getAnalogButtonByTouch(int x, int y);
	std::shared_ptr<EmscriptenRenderingManager> emscriptenRenderingManager;

	void keyboardInput();
	std::optional<long> gamePadID;
public:
	void processInput() override;

	void handleTouch();

	void initializeEmscriptenCanvas() {
		bool isMobile = EmscriptenHandler::is_mobile();

		if (isMobile)
		{
			std::get<0>(screenSize) = EmscriptenHandler::screen_get_width() * 0.9f;
		}
		else {
			std::get<0>(screenSize) = 900;
		}

		std::get<1>(screenSize) = std::get<0>(screenSize) * 3 / 5;
	}

	void initialize() override;

	void loadConfig(std::shared_ptr<GameConfiguration> gameConfiguration) override;
	void saveConfig(std::string key, std::string value) override;

	void setRenderingManager(std::shared_ptr<EmscriptenRenderingManager> emscriptenRenderingManager) { this->emscriptenRenderingManager = emscriptenRenderingManager; }

	void activateCanvasViewPort();
	void activateDisplayViewPort();
	void framebufferSizeCallback(int width, int height);
};

#endif

#endif