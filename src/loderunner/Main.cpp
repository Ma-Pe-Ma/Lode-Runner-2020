#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#if defined __EMSCRIPTEN__
	#include <GLES3/gl3.h>
	#include <emscripten/emscripten.h>
	#include "iocontext/EmscriptenIOContext.h"
	#include "Audio/OpenALAudio.h"
#else 
	#include <glad/glad.h>
	#include "iocontext/GlfwIOContext.h"		
	#include "Audio/RTAudioContext.h"
#endif

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "iocontext/GameConfiguration.h"
#include "Rendering/RenderingManager.h"

#include "States/State.h"
#include "States/StateContext.h"

#include "Player.h"
#include "GameTime.h"

inline void handleImGuiConfigurer();
inline void finalizeImguiWindow();
inline void update();

std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();

std::shared_ptr<StateContext> stateContext;
std::shared_ptr<GameContext> gameContext;
std::shared_ptr<GameConfiguration> gameConfiguration;
std::shared_ptr<GlfwIOContext> glfwIOContext;
std::shared_ptr<Audio> audio;

std::vector<std::string> soundNames = {
	"gold", "dig", "dig_prev", "death", "everygold",
	"mainmenu", "gameover", "gameplay", "intro", "step1",
	"step2", "ladder1", "ladder2", "outro", "pause",
	"pole1", "pole2", "fall"
};

int main(int argc, char**argv) {

#if defined __EMSCRIPTEN__
	audio = std::make_shared<OpenALAudio>();
#else
	audio = std::make_shared<RTAudioContext>();
#endif
	audio->setAudioFileNames("Assets/SFX/", soundNames);
	audio->initialize();

	gameConfiguration = std::make_shared<GameConfiguration>();

#ifdef __EMSCRIPTEN__
	glfwIOContext = std::make_shared<EmscriptenIOContext>();
#else
	glfwIOContext = std::make_shared<GlfwIOContext>();
#endif

	glfwIOContext->loadConfig(gameConfiguration);

	GameTime::setFPS(gameConfiguration->getFramesPerSec());

	//starting championship mode with command line
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "championship") == 0 || strcmp(argv[i], "Championship") == 0) {
			gameConfiguration->setGameVersion(1);
			break;
		}
	}

	glfwIOContext->initialize();

	std::shared_ptr<RenderingManager> renderingManager = std::make_shared<RenderingManager>("./Assets/", gameConfiguration->getMainMenuTextureName(), glfwIOContext);	

	stateContext = std::make_shared<StateContext>();
	stateContext->setRenderingManager(renderingManager);	
	stateContext->setAudio(audio);
	stateContext->initialize();

	gameContext = std::make_shared<GameContext>();
	gameContext->setGameConfiguration(gameConfiguration);
	gameContext->setRenderingManager(renderingManager);
	gameContext->setAudio(audio);
	gameContext->setIOContext(glfwIOContext);

	renderingManager->setGameContext(gameContext);

	stateContext->getGamePlay()->setGameContext(gameContext);
	stateContext->getGenerator()->setGameContext(gameContext);

	stateContext->setIOContext(glfwIOContext);
	stateContext->setGameConfiguration(gameConfiguration);

	prevFrameStart = std::chrono::system_clock::now();	

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop((em_callback_func) update, 60, 1);
#else

	//game loop
	while (!glfwWindowShouldClose(glfwIOContext->getWindow())) {
		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - prevFrameStart;

		if (work_time.count() < 1000.0f / GameTime::getFPS()) {
			std::chrono::duration<double, std::milli> delta_ms(1000.0f / GameTime::getFPS() - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		prevFrameStart = std::chrono::system_clock::now();

		update();
	}

#endif
	audio->terminate();
	renderingManager->terminate();

	glfwTerminate();
	return 0;
}


#if defined __EMSCRIPTEN__
bool showImguiWindow = true;
#else 
bool showImguiWindow = false;
#endif

void update() {	
	GameTime::calculateTimeValues(glfwGetTime());
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glfwIOContext->processInput();	

	handleImGuiConfigurer();

	stateContext->update(GameTime::getCurrentFrame());
	glfwIOContext->handleScreenRecording();	

	finalizeImguiWindow();

	glfwSwapBuffers(glfwIOContext->getWindow());
	glfwPollEvents();
}

void handleImGuiConfigurer() {	
	if (glfwIOContext->getConfigButton().simple()) {
		showImguiWindow = !showImguiWindow;
	}

	if (showImguiWindow) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		//ImVec2 windowSize = ImVec2(GLHelper::SCR_WIDTH / 8, GLHelper::SCR_HEIGHT / 6);
		
#ifdef __EMSCRIPTEN__
		ImVec2 windowSize = ImVec2(220, 280);
#else
		ImVec2 windowSize = ImVec2(220, 140);
#endif
		ImGui::SetNextWindowPos(ImVec2(std::get<0>(glfwIOContext->getScreenSize()) / 25, std::get<0>(glfwIOContext->getScreenSize()) / 25), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(windowSize);

		ImGui::Begin("Lode Runner - configurer", &showImguiWindow, ImGuiWindowFlags_NoResize);

		ImGui::Text("Game version");

		if (ImGui::RadioButton("Original", gameConfiguration->getGameVersionPointer(), 0)) {
			gameConfiguration->setGameVersion(0);
			glfwIOContext->saveConfig("levelset", "0");
			gameConfiguration->validateLevel(stateContext->level[stateContext->playerNr]);
		}

		ImGui::SameLine();
		if (ImGui::RadioButton("Championship", gameConfiguration->getGameVersionPointer(), 1)) {
			stateContext->menuCursor = 0;
			gameConfiguration->setGameVersion(1);
			glfwIOContext->saveConfig("levelset", "1");
			gameConfiguration->validateLevel(stateContext->level[stateContext->playerNr]);
		}

		ImGui::PushItemWidth(std::get<0>(glfwIOContext->getScreenSize()) / 20);

		if (ImGui::InputInt("Level", &stateContext->level[0], 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
			gameConfiguration->validateLevel(stateContext->level[stateContext->playerNr]);
		}

		if (ImGui::SliderFloat("Player speed", gameConfiguration->getPlayerSpeedPointer(), 0.0f, 1.0f, "%.2f")) {

			if (gameContext->getPlayer()) {
				gameContext->getPlayer()->setCharSpeed(gameConfiguration->getPlayerSpeed());
			}
		}

		if (ImGui::SliderFloat("Enemy speed", gameConfiguration->getEnemySpeedPointer(), 0.0f, 1.0f, "%.2f")) {
			for (auto& enemy : gameContext->getEnemies()) {	
				enemy->setCharSpeed(gameConfiguration->getEnemySpeed());
			}
		}

#ifdef __EMSCRIPTEN__
		ImGui::NewLine();
		ImGui::Text("Controls:");
		ImGui::Text("\tarrows - moving");
		ImGui::Text("\tq - left dig");
		ImGui::Text("\tw - right dig");
		ImGui::Text("\tspace - level select");
		ImGui::Text("\tenter - pause");
		ImGui::Text("\tc - show/hide this window");
#endif

		ImGui::PopItemWidth();

		ImGui::End();
	}
}

void finalizeImguiWindow()
{
	if (showImguiWindow)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}