#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

#if defined __EMSCRIPTEN__
	#include <GLES3/gl3.h>
	#include <emscripten/emscripten.h>
#else 
	#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <Shader.h>

#include <RtAudio.h>

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMediaHelper.h"
#include "MultiMediaRecording/MultiMedia.h"
#endif

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "GLHelper.h"
#include "Drawing.h"

#include <cstdio>
#include <chrono>
#include <thread>
#include <string.h>

#include "States/State.h"
#include "States/StateContext.h"

#include "GameTime.h"
#include "IOHandler.h"

#include "Player.h"

inline void handleImGuiConfigurer();
inline void update();

std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();

int main(int argc, char**argv) {

#if defined __EMSCRIPTEN__
	Audio::initializeOpenAL();
#else
	//loading configuration file
	loadConfig();

	//starting championship mode with command line
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "championship") == 0 || strcmp(argv[i], "Championship") == 0) {
			levelFileName = "Assets/Level/ChampionshipLevels.txt";
			gameVersion = 1;
			break;
		}
	}
	
	RtAudio dac = RtAudio(SOUND_API);

	if (dac.getDeviceCount() < 1) {
		std::cout << "\nNo audio devices found!\n";
		exit(0);
	}

	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	
	unsigned int bufferFrames = FRAMES_PER_BUFFER;
	try {
		dac.openStream(&parameters, NULL, RTAUDIO_SINT16, SAMPLE_RATE, &bufferFrames, &Audio::rtAudioVorbis, (void *)Audio::sfx);
		dac.startStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
		exit(0);
	}

#endif	

	Audio::openAudioFiles(Audio::soundNames);

	glfwInit();

	glfwSetErrorCallback(GLHelper::errorCallback);
#if defined __EMSCRIPTEN__
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLHelper::window = glfwCreateWindow(GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT, u8"Lode Runner 2020 - Margitai Péter Máté", NULL, NULL);
	glfwSetWindowPosCallback(GLHelper::window, GLHelper::window_pos_callback);
	glfwSetWindowPos(GLHelper::window, GLHelper::windowPosX, GLHelper::windowPosY);
	//GLHelper::framebuffer_size_callback(GLHelper::window, GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT);

	if (GLHelper::window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		int a;
		std::cin >> a;
		return -1;
	}
	
	glfwSetInputMode(GLHelper::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwMakeContextCurrent(GLHelper::window);
	glfwSetFramebufferSizeCallback(GLHelper::window, GLHelper::framebuffer_size_callback);

#if !defined __EMSCRIPTEN__
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		int a;
		std::cin >> a;
		return -1;
	}
#endif

	std::string mainMenuTextureName = "Assets/Texture/MainMenu.png";

	if (usCover) {
		mainMenuTextureName = "Assets/Texture/MainMenuU.png";
	}

	GLHelper::initialize(mainMenuTextureName);

#ifdef VIDEO_RECORDING
	AudioParameters* audioIn = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);
	AudioParameters* audioOut = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);

	VideoParameters* videoIn = new VideoParameters(GLHelper::viewPortWidth, GLHelper::viewPortHeight, AV_CODEC_ID_NONE, 400000, AV_PIX_FMT_RGB24, STREAM_FRAME_RATE);
	VideoParameters* videoOut = new VideoParameters(0, recordingHeight, AV_CODEC_ID_H264, 400000, AV_PIX_FMT_YUV420P, STREAM_FRAME_RATE);

	MultiMedia media(audioIn, audioOut, videoIn, videoOut);
	media.setGLViewPortReferences(&GLHelper::viewPortX, &GLHelper::viewPortY, &GLHelper::viewPortWidth, &GLHelper::viewPortHeight);
	media.setGenerateName(generateNewVideoName);
	media.setVideoOutputSizeWanted(0, recordingHeight);
	Audio::multiMedia = &media;
	GLHelper::multiMedia = &media;
#endif

	State::initialize(new StateContext());
	GameState::initialize(State::stateContext->gamePlay);

	prevFrameStart = std::chrono::system_clock::now();

	GLHelper::framebuffer_size_callback(GLHelper::window, GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//int major, minor, rev;
	//glfwGetVersion(&major, &minor, &rev);
	//std::cout << "GLFW - major: " << major << ", minor: " << minor << ", rev: " << rev << std::endl;

	ImGui_ImplGlfw_InitForOpenGL(GLHelper::window, true);

#ifdef __EMSCRIPTEN__
	ImGui_ImplOpenGL3_Init("#version 300 es");
#else
	ImGui_ImplOpenGL3_Init("#version 460 core");
#endif

	ImGui::StyleColorsDark();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop((em_callback_func) update, 60, 1);
#else
	//game loop
	while (!glfwWindowShouldClose(GLHelper::window)) {
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
	GLHelper::terminate();

	glfwTerminate();
	return 0;
}

void update() {	
	GameTime::calculateTimeValues(glfwGetTime());
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	processInput(GLHelper::window);

#ifndef __EMSCRIPTEN__
	if (lAlt.continuous() && enter.simple()) {
		GLHelper::fullscreenSwitch();
	}
#endif // !__EMSCRIPTEN__

	State::stateContext->update(GameTime::getCurrentFrame());

	//take a screenshot
	if (pButton.simple()) {
		GLHelper::screenCapture();
	}

#ifdef VIDEO_RECORDING
	//With the help of this function you can record videos
	media.recordAndControl(REC.simple());
#endif
	handleImGuiConfigurer();

	glfwSwapBuffers(GLHelper::window);
	glfwPollEvents();
}

void setCorrectLevel() {
	int max = gameVersion == 0 ? 150 : 51;

	State::stateContext->level[0] = State::stateContext->level[0] > max ? max : State::stateContext->level[0];
	State::stateContext->level[0] = State::stateContext->level[0] < 1 ? 1 : State::stateContext->level[0];

	State::stateContext->level[1] = State::stateContext->level[0];
}

void handleImGuiConfigurer() {
#if defined __EMSCRIPTEN__
	static bool windowOpen = true;
#else 
	static bool windowOpen = false;
#endif
	
	if (configButton.simple()) {
		windowOpen = !windowOpen;
	}

	if (windowOpen) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		//ImVec2 windowSize = ImVec2(GLHelper::SCR_WIDTH / 8, GLHelper::SCR_HEIGHT / 6);
		
#ifdef __EMSCRIPTEN__
		ImVec2 windowSize = ImVec2(220, 260);
#else
		ImVec2 windowSize = ImVec2(220, 140);
#endif
		ImGui::SetNextWindowSize(windowSize);

		ImGui::Begin("Lode Runner - configurer", &windowOpen, ImGuiWindowFlags_NoResize);

		ImGui::Text("Game version");

		if (ImGui::RadioButton("Original", &gameVersion, 0)) {
			levelFileName = "Assets/Level/OriginalLevels.txt";
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GLHelper::originalMenu);
			GLHelper::mainShader->setInt("textureA", 2);
			setCorrectLevel();
		}

		ImGui::SameLine();
		if (ImGui::RadioButton("Championship", &gameVersion, 1)) {
			State::stateContext->menuCursor = 0;
			levelFileName = "Assets/Level/ChampionshipLevels.txt";
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GLHelper::championshipMenu);
			GLHelper::mainShader->setInt("textureA", 2);
			setCorrectLevel();
		}

		ImGui::PushItemWidth(GLHelper::SCR_WIDTH / 20);

		if (ImGui::InputInt("Level", &State::stateContext->level[0], 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
			setCorrectLevel();
		}

		if (ImGui::SliderFloat("Player speed", &Enemy::playerSpeed, 0.0f, 1.0f, "%.2f")) {
			Enemy::player->updateCharSpeed();
		}

		if (ImGui::SliderFloat("Enemy speed", &Enemy::enemySpeed, 0.0f, 1.0f, "%.2f")) {
			for (auto& enemy : Enemy::enemies) {
				enemy->updateCharSpeed();
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
#endif

		ImGui::PopItemWidth();

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}