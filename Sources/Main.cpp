#ifndef ANDROID_VERSION

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SHADER/shader.h>

#include <RtAudio/RtAudio.h>

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMediaHelper.h"
#include "MultiMediaRecording/MultiMedia.h"
#endif

#include "GLHelper.h"
#include "Drawing.h"

#include <cstdio>
#include <chrono>
#include <thread>

#include "States/State.h"
#include "States/StateContext.h"

#include "GameTime.h"
#include "IOHandler.h"

int main(int argc, char**argv) {
#ifndef RELEASE_VERSION
	HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);
	ShowWindow(hWnd, SW_SHOW);
#endif

	//loading configuration file
	loadConfig();

	//starting championship mode with command line
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "championship") == 0 || strcmp(argv[i], "Championship") == 0) {
			levelFileName = "Assets/level/ChampionshipLevels.txt";
			championship = true;
			break;
		}
	}	

	Audio::openAudioFiles(Audio::soundNames);
	RtAudio dac = RtAudio(RtAudio::WINDOWS_WASAPI);

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

	glfwInit();

	glfwSetErrorCallback(GLHelper::errorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLHelper::window = glfwCreateWindow(GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT, "Lode Runner 2020 - Margitai Peter", NULL, NULL);
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
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		int a;
		std::cin >> a;
		return -1;
	}

	std::string mainMenuTextureName = "Assets/Texture/MainMenu.png";
	if (championship) {
		mainMenuTextureName = "Assets/Texture/Championship.png";
	}		
	else if (usCover) {
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

	std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();
	
	GLHelper::framebuffer_size_callback(GLHelper::window, GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT);

	//game loop
	while (!glfwWindowShouldClose(GLHelper::window)) {
		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - prevFrameStart;

		if (work_time.count() < 1000.0f/GameTime::getFPS()) {
			std::chrono::duration<double, std::milli> delta_ms(1000.0f/GameTime::getFPS() - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		prevFrameStart = std::chrono::system_clock::now();		
		GameTime::calculateTimeValues(glfwGetTime());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(GLHelper::window);

		if (lAlt.continuous() && enter.simple()) {
			GLHelper::fullscreenSwitch();
		}			

		State::stateContext->update(GameTime::getCurrentFrame());

		//take a screenshot
		if (pButton.simple()) {
			GLHelper::screenCapture();
		}			

#ifdef VIDEO_RECORDING
		//With the help of this function you can record videos
		media.recordAndControl(REC.simple());
#endif

		glfwSwapBuffers(GLHelper::window);
		glfwPollEvents();
	}

	GLHelper::terminate();

	glfwTerminate();
	return 0;
}

#endif // !ANDROID_VERSION