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
		void set_cookie(const char*, const char*);
	}
}

class EmscriptenIOContext : public GlfwIOContext {

public:
	void processInput() override;

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

	void loadConfig(std::shared_ptr<GameConfiguration> gameConfiguration) override;
	void saveConfig(std::string key, std::string value) override;
};

#endif

#endif