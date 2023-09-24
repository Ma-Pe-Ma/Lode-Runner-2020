#include "EmscriptenIOContext.h"

#include <GLFW/glfw3.h>
#include <cmath>

#ifdef __EMSCRIPTEN__
#include "rendering/EmscriptenRenderingManager.h"

namespace EmscriptenHandler {
	EM_JS(int, canvas_get_width, (), {
		return canvas.getBoundingClientRect().width;
		});

	EM_JS(int, canvas_get_height, (), {
		return canvas.getBoundingClientRect().height;
		});

	EM_JS(int, screen_get_width, (), {
		return screen.width;
		});

	EM_JS(int, screen_get_height, (), {
		return screen.height;
		});

	EM_JS(int, window_get_width, (), {
	return window.width;
		});

	EM_JS(int, window_get_height, (), {
		return window.height;
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

	EM_JS(int, getVertexMaxUniformNumber, (), {
		const gl2 = Module.canvas.getContext('webgl2');
		return gl2.getParameter(gl2.MAX_VERTEX_UNIFORM_VECTORS);
		});

	EM_JS(int, getFragmentMaxUniformNumber, (), {
		const gl2 = Module.canvas.getContext('webgl2');
		return gl2.getParameter(gl2.MAX_FRAGMENT_UNIFORM_VECTORS);
		});
}

void EmscriptenIOContext::processInput() {
	if (EmscriptenHandler::is_mobile()) {
		for (auto it = buttonStateMap.begin(); it != buttonStateMap.end(); it++)
		{
			it->first->detect(it->second);
		}
	}
	else {
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
}

void EmscriptenIOContext::loadConfig(std::shared_ptr<GameConfiguration> gameConfiguration)
{
	const std::string keyPrefix = "LR_";

	std::string cookies = EmscriptenHandler::get_cookies();
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
	EmscriptenHandler::set_cookie(key.c_str(), value.c_str());
}

void EmscriptenIOContext::initialize() {
	initializeEmscriptenCanvas();
	GlfwIOContext::initialize();

	emscripten_set_orientationchange_callback(this, false, (em_orientationchange_callback_func) [](int eventType, const EmscriptenOrientationChangeEvent* orientationChangeEvent, void* userData) -> EM_BOOL {
		//EmscriptenIOContext* self = static_cast<EmscriptenIOContext*>(userData);

		EmscriptenFullscreenChangeEvent efce;
		emscripten_get_fullscreen_status(&efce);

		if (efce.isFullscreen) {
			emscripten_exit_fullscreen();
		}

		return true;
	});

	emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, (em_fullscreenchange_callback_func) [](int eventType, const EmscriptenFullscreenChangeEvent* fullscreenChangeEvent, void* userData) -> EM_BOOL {
		int width;
		int height;
		emscripten_get_canvas_element_size("canvas", &width, &height);
		
		return true;
		});

	emscripten_set_touchstart_callback("canvas", this, true, (em_touch_callback_func) [](int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) -> EM_BOOL {
		EmscriptenIOContext* self = static_cast<EmscriptenIOContext*>(userData);
			
		for (int i = 0; i < touchEvent->numTouches; i++)
		{
			const EmscriptenTouchPoint& tp = touchEvent->touches[i];

			if (self->analogID == -1) {
				auto analogInput = self->getAnalogButtonByTouch(tp.clientX, tp.clientY);

				auto xButton = std::get<0>(analogInput);
				auto yButton = std::get<1>(analogInput);

				if (xButton != nullptr || yButton != nullptr) {
					if (xButton) {
						self->buttonStateMap[xButton] = true;
					}

					if (yButton) {
						self->buttonStateMap[yButton] = true;
					}

					self->analogID = tp.identifier;
					continue;
				}
			}

			for (auto it = self->sizedPositionMap.begin(); it != self->sizedPositionMap.end(); it++) {					
				auto borders = it->second;

				int left = std::get<0>(borders);
				int right = std::get<1>(borders);

				int up = std::get<2>(borders);
				int down = std::get<3>(borders);				

				if (left < tp.clientX && tp.clientX < right && up < tp.clientY && tp.clientY < down) {
					self->buttonMap[tp.identifier] = it->first;
					self->buttonStateMap[it->first] = true;
					break;
				}
			}
		}

		return true;
	});

	emscripten_set_touchend_callback("canvas", this, true, (em_touch_callback_func) [](int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) -> EM_BOOL {
		EmscriptenIOContext* self = static_cast<EmscriptenIOContext*>(userData);

		for (int i = 0; i < touchEvent->numTouches; i++)
		{
			const EmscriptenTouchPoint& tp = touchEvent->touches[i];
			
			if (tp.identifier == self->analogID) {
				self->analogID = -1;

				self->buttonStateMap[&self->leftButton] = false;
				self->buttonStateMap[&self->rightButton] = false;
				self->buttonStateMap[&self->up] = false;
				self->buttonStateMap[&self->down] = false;
			}
			else {
				auto it = self->buttonMap.find(tp.identifier);

				if (it != self->buttonMap.end()) {
					self->buttonStateMap[it->second] = false;
					self->buttonMap.erase(it);
				}
			}
		}
		return true;
	});

	emscripten_set_touchmove_callback("canvas", this, true, (em_touch_callback_func) [](int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) -> EM_BOOL {
		EmscriptenIOContext* self = static_cast<EmscriptenIOContext*>(userData);		

		for (int i = 0; i < touchEvent->numTouches; i++)
		{
			const EmscriptenTouchPoint& tp = touchEvent->touches[i];
			
			if (tp.identifier == self->analogID)
			{
				auto analog = self->getAnalogButtonByTouch(tp.clientX, tp.clientY);
				auto x = std::get<0>(analog);
				auto y = std::get<1>(analog);

				self->buttonStateMap[&self->rightButton] = false;
				self->buttonStateMap[&self->leftButton] = false;
				self->buttonStateMap[&self->down] = false;
				self->buttonStateMap[&self->up] = false;

				self->buttonStateMap[x] = true;
				self->buttonStateMap[y] = true;
			}
			else {
				auto it = self->buttonMap.find(tp.identifier);

				if (it != self->buttonMap.end()) {
					auto borders = self->sizedPositionMap[it->second];

					int left = std::get<0>(borders);
					int right = std::get<1>(borders);

					int up = std::get<2>(borders);
					int down = std::get<3>(borders);

					if (left < tp.clientX && tp.clientX < right && up < tp.clientY && tp.clientY < down) {
						self->buttonMap[tp.identifier] = it->second;
						self->buttonStateMap[it->second] = true;
					}
					else {
						self->buttonStateMap[it->second] = false;
					}
				}
			} 
		}

		return true;
	});

	emscripten_set_touchcancel_callback("canvas", this, true, (em_touch_callback_func) [](int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) -> EM_BOOL {
		EmscriptenIOContext* self = static_cast<EmscriptenIOContext*>(userData);

		for (int i = 0; i < touchEvent->numTouches; i++)
		{
			const EmscriptenTouchPoint& tp = touchEvent->touches[i];
			
			if (tp.identifier == self->analogID) {
				self->analogID = -1;

				self->buttonStateMap[&self->leftButton] = false;
				self->buttonStateMap[&self->rightButton] = false;
				self->buttonStateMap[&self->up] = false;
				self->buttonStateMap[&self->down] = false;
			}
			else {
				auto it = self->buttonMap.find(tp.identifier);

				if (it != self->buttonMap.end()) {
					self->buttonStateMap[it->second] = false;
					self->buttonMap.erase(it);
				}
			}
		}

		return true;
	});

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		auto self = static_cast<EmscriptenIOContext*>(glfwGetWindowUserPointer(window));
		self->updateViewPortValues(width, height);

		if (EmscriptenHandler::is_mobile()) {
			self->framebufferSizeCallback(width, height);
		}

		glViewport(std::get<0>(self->viewPortPosition), std::get<1>(self->viewPortPosition), std::get<0>(self->viewPortSize), std::get<1>(self->viewPortSize));
	});
}

std::tuple<Button*, Button*> EmscriptenIOContext::getAnalogButtonByTouch(int absX, int absY) {
	int x = absX - analogCenterX;
	int y = absY - analogCenterY;

	float distance = std::hypotf(x, y);

	if (distance < analogRadius * 0.2f || analogRadius < distance) {
		return std::make_tuple<Button*, Button*>(nullptr, nullptr);
	}

	float angle = std::atan2(y, x);

	Button* xButton = -M_PI / 3 < angle && angle < M_PI / 3 ? &rightButton : angle > M_PI * 2 / 3 || angle < - M_PI * 2 / 3 ? &leftButton : nullptr;
	Button* yButton = M_PI / 6 < angle && angle < M_PI * 5 / 6 ? &down : -M_PI * 5 / 6 < angle && angle < -M_PI / 6 ? &up : nullptr;

	return { xButton, yButton };	
}

void EmscriptenIOContext::activateCanvasViewPort() {
	glViewport(std::get<0>(viewPortPosition), std::get<1>(viewPortPosition), std::get<0>(viewPortSize), std::get<1>(viewPortSize));
}

void EmscriptenIOContext::activateDisplayViewPort() {
	glViewport(0, 0, std::get<0>(screenSize), std::get<1>(screenSize));
}

void EmscriptenIOContext::framebufferSizeCallback(int width, int height) {
	analogCenterX = 0.2f * width;
	analogCenterY = 0.5f * height;
	analogRadius = 0.1f * 1.5f * width;

	//origin is top left
	const std::map <Button*, std::tuple<float, float, float, float>> digitalPositionMap{
		{&space, {0.05f, -0.95f, 0.1f, 0.0f}},
		{&enter, {0.95f, -0.95f, 0.1f, 0.0f}},
		{&leftDigButton, {0.75f, 0.5f, 0.1f, 0.0f}},
		{&rightDigButton, {0.9f, 0.5f, 0.1f, 0.0f}},
		{&configButton, {0.5f, -0.975f, 0.05f, 0.0f}},
	};

	const std::map<Button*, int> textureCoordMap{
		{&down, 0},
		{&leftButton, 1},
		{&rightButton, 2},
		{&up, 3},
		{&leftDigButton, 4},
		{&rightDigButton, 5},
		{&space, 6},
		{&enter, 6},
		{&configButton, 7},
	};

	float touchRenderVertices[TOUCH_BUTTON_NR * 16] = {};
	const std::function<void(int, int, float, float, float, float)> generateVertices = [&touchRenderVertices](int i, int offset, float normalizedLeft, float normalizedRight, float normalizedTop, float normalizedBottom) ->void {
		touchRenderVertices[16 * i + 0] = normalizedLeft;
		touchRenderVertices[16 * i + 1] = normalizedTop;
		touchRenderVertices[16 * i + 4] = normalizedRight;
		touchRenderVertices[16 * i + 5] = normalizedTop;

		touchRenderVertices[16 * i + 8] = normalizedLeft;
		touchRenderVertices[16 * i + 9] = normalizedBottom;
		touchRenderVertices[16 * i + 12] = normalizedRight;
		touchRenderVertices[16 * i + 13] = normalizedBottom;

		float xOffset = (offset % 4) * 0.25f;
		float yOffset = (offset / 4) * 0.5f;

		touchRenderVertices[16 * i + 2] = 0.0f + xOffset;
		touchRenderVertices[16 * i + 3] = 1.0f + yOffset;
		touchRenderVertices[16 * i + 6] = 0.25f + xOffset;
		touchRenderVertices[16 * i + 7] = 1.0f + yOffset;

		touchRenderVertices[16 * i + 10] = 0.0f + xOffset;
		touchRenderVertices[16 * i + 11] = 0.5f + yOffset;
		touchRenderVertices[16 * i + 14] = 0.25f + xOffset;
		touchRenderVertices[16 * i + 15] = 0.5f + yOffset;
	};

	for (auto it = digitalPositionMap.begin(); it != digitalPositionMap.end(); it++)
	{
		auto& borders = it->second;

		//complicated solution for constraining buttons
		float centerX = width * std::get<0>(borders);
		int centerY = std::get<1>(borders) > 0 ? std::get<1>(borders) * height : (height - (1.0f + std::get<1>(borders)) * width);

		int sizeX = width * std::get<2>(borders) / 2;
		int sizeY = sizeX;

		sizedPositionMap[it->first] = { centerX - sizeX, centerX + sizeX, centerY - sizeY, centerY + sizeY };

		//as at touch input the positive y is measured from top to bottom, while at GL the positive is from bottom to top: so it needs to be 
		float newYFactor = std::get<1>(borders) > 0 ? 1.0f - std::get<1>(borders) : 1.0f + std::get<1>(borders);
		float ratio1 = std::get<1>(borders) > 0 ? 1.0f : float(width) / height;

		float normalizedLeft = 2.0f * (std::get<0>(borders) - std::get<2>(borders) / 2) - 1.0f;
		float normalizedRight = 2.0f * (std::get<0>(borders) + std::get<2>(borders) / 2) - 1.0f;
		float normalizedTop = 2.0f * (newYFactor * ratio1 + std::get<2>(borders) / 2 * width / height) - 1.0f;
		float normalizedBottom = 2.0f * (newYFactor * ratio1 - std::get<2>(borders) / 2 * width / height) - 1.0f;;

		int i = std::distance(digitalPositionMap.begin(), it);
		int offset = textureCoordMap.at(it->first);

		generateVertices(i, offset, normalizedLeft, normalizedRight, normalizedTop, normalizedBottom);
	}

	const std::map<Button*, std::tuple<float, float, float, float>> analogPositionMap{
		{&leftButton, { -0.8f, 0.0f, 0.2f, 0.0f} },
		{&rightButton, {-0.4f, 0.0f, 0.2f, 0.0f} },
		{&up, {-0.6f, 0.0f, 0.2f, 0.2f} },
		{&down, {-0.6f, 0.0f, 0.2f, -0.2f} },
	};

	for (auto it = analogPositionMap.begin(); it != analogPositionMap.end(); it++) {
		auto borders = it->second;
		float centerY = std::get<1>(borders) + std::get<3>(borders) * width / height;

		float normalizedLeft = std::get<0>(borders) - std::get<2>(borders) / 2;
		float normalizedRight = std::get<0>(borders) + std::get<2>(borders) / 2;
		float normalizedTop = centerY + std::get<2>(borders) / 2 * width / height;
		float normalizedBottom = centerY - std::get<2>(borders) / 2 * width / height;

		int i = std::distance(analogPositionMap.begin(), it) + digitalPositionMap.size();
		int offset = textureCoordMap.at(it->first);

		generateVertices(i, offset, normalizedLeft, normalizedRight, normalizedTop, normalizedBottom);
	}

	emscriptenRenderingManager->updateTouchButtonRenderer(touchRenderVertices);
};


#endif // __EMSCRIPTEN__