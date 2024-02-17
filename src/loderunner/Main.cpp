#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <thread>

#include "AppContainer.h"

int main(int argc, char**argv) {
	AppContainer appContainer;
	appContainer.initialize();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg((em_arg_callback_func) [](void* appContainer) -> void {
		auto ac = static_cast<AppContainer*>(appContainer);
		ac->update();
	}, &appContainer, 60, 1);
#else
	std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();

	//game loop
	while (appContainer.shouldClose()) {
		const std::chrono::duration<double, std::milli> workTime = std::chrono::system_clock::now() - prevFrameStart;
		const unsigned int frameRate = appContainer.getFrameRate();

		if (workTime.count() < 1000.0f / frameRate) {
			std::chrono::duration<double, std::milli> deltaMS(1000.0f / frameRate - workTime.count());
			auto deltaMSDuration = std::chrono::duration_cast<std::chrono::milliseconds>(deltaMS);
			std::this_thread::sleep_for(std::chrono::milliseconds(deltaMSDuration.count()));
		}

		prevFrameStart = std::chrono::system_clock::now();

		appContainer.update();
	}
#endif	
	return 0;
}