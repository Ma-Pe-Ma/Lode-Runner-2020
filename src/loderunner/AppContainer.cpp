#include "AppContainer.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "gameplay/Player.h"

void AppContainer::initialize()
{
#if defined __EMSCRIPTEN__
	audio = std::make_shared<OpenALAudio>();
#else
	audio = std::make_shared<RTAudioContext>();
#endif
	audio->setAudioFileNames("Assets/SFX/", soundNames);
	audio->initialize();

	gameConfiguration = std::make_shared<GameConfiguration>();

#ifdef __EMSCRIPTEN__
	ioContext = std::make_shared<EmscriptenIOContext>();
#else
	ioContext = std::make_shared<GlfwIOContext>();
#endif

	ioContext->loadConfig(gameConfiguration);
	ioContext->initialize();

	renderingManager = std::make_shared<RenderingManager>("./Assets/", ioContext);

	stateContext = std::make_shared<StateContext>();
	stateContext->setRenderingManager(renderingManager);
	stateContext->setAudio(audio);
	stateContext->initialize();

	gameContext = std::make_shared<GameContext>();
	gameContext->setGameConfiguration(gameConfiguration);
	gameContext->setRenderingManager(renderingManager);
	gameContext->setAudio(audio);
	gameContext->setIOContext(ioContext);

	renderingManager->setGameContext(gameContext);

	stateContext->getGamePlay()->setGameContext(gameContext);
	stateContext->getGenerator()->setGameContext(gameContext);

	stateContext->setIOContext(ioContext);
	stateContext->setGameConfiguration(gameConfiguration);
}

void AppContainer::update()
{
	ioContext->processInput();
	handleImGuiConfigurer();

	stateContext->update();
	ioContext->handleScreenRecording();

	finalizeImguiWindow();
	ioContext->finalizeFrame();
}

void AppContainer::terminate()
{
	audio->terminate();
	renderingManager->terminate();

	glfwTerminate();
}

void AppContainer::handleImGuiConfigurer()
{
	if (ioContext->getConfigButton().simple()) {
		showImguiWindow = !showImguiWindow;
	}

	if (showImguiWindow) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		//ImVec2 windowSize = ImVec2(GLHelper::SCR_WIDTH / 8, GLHelper::SCR_HEIGHT / 6);

#ifndef NDEBUG
		ImVec2 windowSize = ImVec2(220, 310);
#else
		ImVec2 windowSize = ImVec2(220, 280);
#endif
		ImGui::SetNextWindowPos(ImVec2(std::get<0>(ioContext->getScreenSize()) / 25, std::get<0>(ioContext->getScreenSize()) / 25), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(windowSize);

		ImGui::Begin("Lode Runner - configurer", &showImguiWindow, ImGuiWindowFlags_NoResize);

		ImGui::Text("Game version");

		if (ImGui::RadioButton("Original", gameConfiguration->getGameVersionPointer(), 0)) {
			gameConfiguration->setGameVersion(0);
			ioContext->saveConfig("levelset", "0");
			gameConfiguration->validateLevel(stateContext->level[stateContext->playerNr]);
		}

		ImGui::SameLine();
		if (ImGui::RadioButton("Championship", gameConfiguration->getGameVersionPointer(), 1)) {
			stateContext->menuCursor = 0;
			gameConfiguration->setGameVersion(1);
			ioContext->saveConfig("levelset", "1");
			gameConfiguration->validateLevel(stateContext->level[stateContext->playerNr]);
		}

		ImGui::PushItemWidth(std::get<0>(ioContext->getScreenSize()) / 20);

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

#ifndef NDEBUG	
		ImGui::Checkbox("Control enemies", gameConfiguration->getEnemyDebugState());
#endif
		ImGui::NewLine();
		ImGui::Text("Controls:");
		ImGui::Text("\tarrows - moving");
		ImGui::Text("\tq - left dig");
		ImGui::Text("\tw - right dig");
		ImGui::Text("\tspace - level select");
		ImGui::Text("\tesc/enter - pause");
		ImGui::Text("\tc - show/hide this window");

		ImGui::PopItemWidth();

		ImGui::End();
	}
}

void AppContainer::finalizeImguiWindow()
{
	if (showImguiWindow)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

bool AppContainer::shouldClose()
{
	return ioContext->shouldClose();
}

unsigned int AppContainer::getFrameRate()
{
	return gameConfiguration->getFramesPerSec();
}