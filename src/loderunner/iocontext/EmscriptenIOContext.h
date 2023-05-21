#ifndef EMSCRIPTENIOCONTEXT_H
#define EMSCRIPTENIOCONTEXT_H

#include "GlfwIOContext.h"

#include <iostream>
#include <sstream>
#include <regex>

#include "GameConfiguration.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
namespace EmscriptenHelper {
	extern "C" {
		int canvas_get_width();
		int canvas_get_height();

		int screen_get_width();
		int screen_get_height();

		bool is_mobile();

		char* get_cookies();
		void set_cookie(std::string, std::string);
	}
}

class EmscriptenIOContext : public GlfwIOContext {

public:
	void processInput() override {
		configButton.detect(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);

		leftButton.detect(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
		rightButton.detect(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
		up.detect(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
		down.detect(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
		rightDigButton.detect(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
		leftDigButton.detect(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
		enter.detect(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
		space.detect(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
	}

	void initializeEmscriptenCanvas() {
		bool isMobile = EmscriptenHelper::is_mobile();

		if (isMobile)
		{
			std::get<0>(screenSize) = EmscriptenHelper::screen_get_width() * 0.9f;
		}
		else {
			std::get<0>(screenSize) = 900;
		}

		std::get<1>(screenSize) = std::get<0>(screenSize) * 3 / 5;
	}

	void initialize() override {
		initializeEmscriptenCanvas();
		GlfwIOContext::initialize();
	}

	void loadConfig(std::shared_ptr<GameConfiguration> gameConfiguration) override
	{
		std::string cookies = EmscriptenHelper::get_cookies();
		std::istringstream cookieStream(cookies);
		std::string keyValuePair;

		while (std::getline(cookieStream, keyValuePair, ';')) {
			std::string key = keyValuePair.substr(0, keyValuePair.find("="));
			key = std::regex_replace(key, std::regex("^ +"), "");

			std::string value = keyValuePair.substr(keyValuePair.find("=") + 1);
			value = std::regex_replace(value, std::regex("^ +"), "");

			configMap[key] = value;
		}

		gameConfiguration->setGameVersion(getIntByKey("levelset", 0));
		gameConfiguration->setStartingLevel(getIntByKey("levelNr", 1));
	}

	void saveConfig(std::string key, std::string value) override
	{
		EmscriptenHelper::set_cookie(key, value);
	}
};

#endif

#endif