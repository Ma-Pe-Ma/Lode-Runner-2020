#include "EmscriptenIOContext.h"

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
namespace EmscriptenHelper {
	EM_JS(int, canvas_get_width, (), {
		return canvas.width;
		});

	EM_JS(int, canvas_get_height, (), {
		return canvas.height;
		});

	EM_JS(int, screen_get_width, (), {
		return screen.width;
		});

	EM_JS(int, screen_get_height, (), {
		return screen.height;
		});

	EM_JS(bool, is_mobile, (), {
		var a = false;

		if (navigator.userAgent.match(/Android/i)
			 || navigator.userAgent.match(/webOS/i)
			 || navigator.userAgent.match(/iPhone/i)
			 || navigator.userAgent.match(/iPad/i)
			 || navigator.userAgent.match(/iPod/i)
			 || navigator.userAgent.match(/BlackBerry/i)
			 || navigator.userAgent.match(/Windows Phone/i)
			 ) {
				a = true;
		}

		return a;
		});

	EM_JS(void, set_cookie, (const char* key, const char* value), {
			//console.log("Setting cookie: " + UTF8ToString(key) + "=" + UTF8ToString(value) + ";");
			document.cookie = UTF8ToString(key) + "=" + UTF8ToString(value) + ";path=/;";
		});
	
	EM_JS(char*, get_cookies, (), {
			//console.log("Getting cookies: " + document.cookie);
			return stringToNewUTF8(document.cookie);
		});
}

void EmscriptenIOContext::processInput() {
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

void EmscriptenIOContext::loadConfig(std::shared_ptr<GameConfiguration> gameConfiguration)
{
	const std::string keyPrefix = "LR_";

	std::string cookies = EmscriptenHelper::get_cookies();
	std::istringstream cookieStream(cookies);
	std::string keyValuePair;

	while (std::getline(cookieStream, keyValuePair, ';')) {
		std::string key = keyValuePair.substr(0, keyValuePair.find("="));
		key = key.substr(key.find(keyPrefix) + keyPrefix.length());
		key = std::regex_replace(key, std::regex("^ +"), "");

		std::string value = keyValuePair.substr(keyValuePair.find("=") + 1);
		value = std::regex_replace(value, std::regex("^ +"), "");

		configMap[key] = value;
	}

	gameConfiguration->setGameVersion(getIntByKey("levelset", 0));
	gameConfiguration->setStartingLevel(getIntByKey("levelNr", 1));
}

void EmscriptenIOContext::saveConfig(std::string key, std::string value)
{
	const std::string keyPrefix = "LR_";
	key = keyPrefix + key;
	EmscriptenHelper::set_cookie(key.c_str(), value.c_str());
}

#endif // __EMSCRIPTEN__