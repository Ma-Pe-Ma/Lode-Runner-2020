#include "AppContainer.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

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
		auto translation = gameConfiguration->getTranslation();

#ifndef NDEBUG
		ImVec2 windowSize = ImVec2(220, 430);
#else
		ImVec2 windowSize = ImVec2(220, 400);
#endif
		auto screenSize = ioContext->getScreenParameters().screenSize;
		ImGui::SetNextWindowPos(ImVec2(std::get<0>(screenSize) / 25, std::get<0>(screenSize) / 25), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(windowSize);

		std::string windowTitle = translation->getTranslationString("imguiTitle") + "###mainWindow";
		ImGui::Begin(windowTitle.c_str(), &stateContext->getShowImGuiWindow(), ImGuiWindowFlags_NoResize);

		ImGui::Text(translation->getTranslationString("gameVersion").c_str());

		int index = 0;
		ImGui::Indent(10.0f);
		for (auto it = gameConfiguration->configurations.begin(); it != gameConfiguration->configurations.end(); it++) {
			std::string name = std::vformat("{0} ({1})", std::make_format_args(std::get<4>(it->second), std::get<1>(it->second)));

			if (ImGui::RadioButton(name.c_str(), gameConfiguration->getGameVersionPointer(), index)) {				
				gameConfiguration->setGameVersion(index);
				ioContext->saveConfig("levelset", index);
				gameConfiguration->validateLevel(stateContext->getCurrentLevel());

				if (stateContext->getCurrentState() == stateContext->getMainMenu()) {
					stateContext->getMainMenu()->setTexts();
				}
			}
			index += 1;
		}
		ImGui::Unindent(10.0f);

		ImGui::PushItemWidth(std::get<0>(screenSize) / 20);

		if (ImGui::InputInt(translation->getTranslationString("level").c_str(), &stateContext->getCurrentLevel(), 0, 0)) {
			gameConfiguration->validateLevel(stateContext->getCurrentLevel());
		}

		ImGui::SliderFloat(translation->getTranslationString("playerSpeed").c_str(), gameConfiguration->getPlayerSpeedPointer(), 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat(translation->getTranslationString("enemySpeed").c_str(), gameConfiguration->getEnemySpeedPointer(), 0.0f, 1.0f, "%.2f");

#ifndef NDEBUG	
		ImGui::Checkbox("Control enemies", gameConfiguration->getEnemyDebugState());
#endif
		ImGui::Text(translation->getTranslationString("controls").c_str());
		auto arrows = translation->getTranslationString("arrows");
		ImGui::Text(std::vformat("\t{0}", std::make_format_args(arrows)).c_str());
		auto leftDig = translation->getTranslationString("leftDig");
		ImGui::Text(std::vformat("\tq - {0}", std::make_format_args(leftDig)).c_str());
		auto rightDig = translation->getTranslationString("rightDig");
		ImGui::Text(std::vformat("\tw - {0}", std::make_format_args(rightDig)).c_str());
		auto space = translation->getTranslationString("space");
		ImGui::Text(std::vformat("\tspace - {0}", std::make_format_args(space)).c_str());
		auto escEnter = translation->getTranslationString("escEnter");
		ImGui::Text(std::vformat("\tesc/enter - {0}", std::make_format_args(escEnter)).c_str());
		auto cButton = translation->getTranslationString("cButton");
		ImGui::Text(std::vformat("\tc - {0}", std::make_format_args(cButton)).c_str());

		ImGui::PopItemWidth();

		ImGui::Text(translation->getTranslationString("language").c_str());

		index = 0;
		for (auto& language : gameConfiguration->getLanguages()) {
			if (ImGui::RadioButton(language.c_str(), translation->getCurrentLanguageIndex(), index)) {
				auto newLanguage = gameConfiguration->changeLanguage(index);
				ioContext->saveConfig("language", newLanguage);
				if (stateContext->getCurrentState() == stateContext->getMainMenu()) {
					stateContext->getMainMenu()->setTexts();
				}
			}

			++index;
			ImGui::SameLine();
		}

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