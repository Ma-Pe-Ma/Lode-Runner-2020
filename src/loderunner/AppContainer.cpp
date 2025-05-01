#include "AppContainer.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "gameplay/Player.h"

#ifdef __EMSCRIPTEN__
#include "iocontext/EmscriptenIOContext.h"
#include "iocontext/rendering/EmscriptenRenderingManager.h"
#else
#include "iocontext/GlfwIOContext.h"
#include "iocontext/rendering/RenderingManager.h"
#endif

void AppContainer::initialize()
{
#ifdef __EMSCRIPTEN__
	audio = std::make_shared<OpenALAudioContext>();
#else
	audio = std::make_shared<RTAudioContext>();
#endif
	audio->setAudioFileNames("assets/sfx/", soundNames);
	audio->initialize();

	gameConfiguration = std::make_shared<GameConfiguration>();

#ifdef __EMSCRIPTEN__
	ioContext = std::make_shared<EmscriptenIOContext>();
#else
	ioContext = std::make_shared<GlfwIOContext>();
#endif

	ioContext->loadConfig(gameConfiguration);
	ioContext->initialize();

#ifdef VIDEO_RECORDING
	ioContext->setAudioContext(audio);
#endif

#ifdef __EMSCRIPTEN__
	renderingManager = std::make_shared<EmscriptenRenderingManager>("./assets/", ioContext);
	(std::static_pointer_cast<EmscriptenIOContext>(ioContext))->setRenderingManager(std::static_pointer_cast<EmscriptenRenderingManager>(renderingManager));
#else
	renderingManager = std::make_shared<RenderingManager>("./assets/", ioContext);
#endif
	renderingManager->createShaders();

	gameContext = std::make_shared<GameContext>();
	gameContext->setGameConfiguration(gameConfiguration);
	gameContext->setRenderingManager(renderingManager);
	gameContext->setAudio(audio);
	gameContext->setIOContext(ioContext);

	stateContext = std::make_shared<StateContext>();
	stateContext->setRenderingManager(renderingManager);
	stateContext->setAudio(audio);
	stateContext->setIOContext(ioContext);
	stateContext->setGameConfiguration(gameConfiguration);
	stateContext->getGamePlay()->setGameContext(gameContext);
	stateContext->getGenerator()->setGameContext(gameContext);
	stateContext->initialize();
}

void AppContainer::update()
{
	ioContext->initFrame();
	this->handleImGuiConfigurer();

	stateContext->update();

	ioContext->handleScreenRecording();
	ioContext->finalizeFrame();
}

void AppContainer::terminate()
{
	audio->terminate();
	renderingManager->terminate();
	ioContext->terminate();	
}

void AppContainer::handleImGuiConfigurer()
{
	if (ioContext->getButtonInputs().config.simple()) {
#ifdef __EMSCRIPTEN__
		if (EmscriptenHandler::is_mobile()) {
			int* gameVersion = gameConfiguration->getGameVersionPointer();
			++(*gameVersion);

			if (*gameVersion == 1) {
				stateContext->getMenuCursor() = 0;
			}

			gameConfiguration->setGameVersion(*gameVersion);
			ioContext->saveConfig("levelset", *gameVersion);
			gameConfiguration->validateLevel(stateContext->getCurrentLevel());
		}
		else {
			stateContext->getShowImGuiWindow() = !stateContext->getShowImGuiWindow();
		}		
#else
		stateContext->getShowImGuiWindow() = !stateContext->getShowImGuiWindow();
#endif // __EMSCRIPTEN__
	}

	if (stateContext->getShowImGuiWindow()) {
#ifndef NDEBUG
		ImVec2 windowSize = ImVec2(220, 400);
#else
		ImVec2 windowSize = ImVec2(220, 370);
#endif
		auto screenSize = ioContext->getScreenParameters().screenSize;
		ImGui::SetNextWindowPos(ImVec2(std::get<0>(screenSize) / 25, std::get<0>(screenSize) / 25), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(windowSize);

		ImGui::Begin("Lode Runner - configurer", &stateContext->getShowImGuiWindow(), ImGuiWindowFlags_NoResize);

		ImGui::Text("Game version");

		int index = 0;
		ImGui::Indent(10.0f);
		for (auto it = gameConfiguration->configurations.begin(); it != gameConfiguration->configurations.end(); it++) {
			std::string name = std::vformat("{0} ({1})", std::make_format_args(std::get<3>(it->second), std::get<1>(it->second)));

			if (ImGui::RadioButton(name.c_str(), gameConfiguration->getGameVersionPointer(), index)) {
				gameConfiguration->setGameVersion(index);
				ioContext->saveConfig("levelset", index);
				gameConfiguration->validateLevel(stateContext->getCurrentLevel());
				stateContext->getMainMenu()->setTexts();
			}
			index += 1;
		}
		ImGui::Unindent(10.0f);

		ImGui::PushItemWidth(std::get<0>(screenSize) / 20);

		if (ImGui::InputInt("Level", &stateContext->getCurrentLevel(), 0, 0)) {
			gameConfiguration->validateLevel(stateContext->getCurrentLevel());
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

		ImGui::Text("Language");

		index = 0;
		for (auto& language : gameConfiguration->getLanguages()) {
			if (ImGui::RadioButton(language.c_str(), gameConfiguration->getTranslation()->getCurrentLanguageIndex(), index)) {
				auto newLanguage = gameConfiguration->changeLanguage(index);
				ioContext->saveConfig("language", newLanguage);
				//stateContext->getMainMenu()->setTexts();
			}

			++index;
			ImGui::SameLine();
		}

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

bool AppContainer::shouldClose()
{
	return ioContext->shouldClose();
}

unsigned int AppContainer::getFrameRate()
{
	return gameConfiguration->getFramesPerSec();
}