#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SHADER/shader_m.h>

#include <RtAudio/RtAudio.h>

//#define RELEASE_VERSION
#define VIDEO_RECORDING

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMediaHelper.h"
#include "MultiMediaRecording/MultiMedia.h"
#endif

#include "GLHelper.h"
#include "Variables.h"
#include "Functions.h"
#include "Structure.h"

#include <cstdio>
#include <chrono>
#include <thread>

int main(int argc, char**argv) {
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

#ifdef RELEASE_VERSION
	ShowWindow(hWnd, SW_HIDE);
#else
	ShowWindow(hWnd, SW_SHOW);
#endif

	//fruits? - couldn't find ripped textures :(
	//demo in main menu
	//prevention sound should be differently timed
	//falling sound is not long enough
	//put golds and guards into std::vector

	//loading configuration file
	loadConfig();

	//starting championship mode with command line
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "championship") == 0 || strcmp(argv[i], "Championship") == 0) {

			levelFileName = "level/lodeRunner.champLevel.txt";
			championShip = true;

			if (level[0] > 51) {
				level[0] = 51;
				level[1] = 51;
			}

			break;
		}
	}	

	Audio::OpenAudioFiles(Audio::soundNames);
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
		dac.openStream(&parameters, NULL, RTAUDIO_SINT16, SAMPLE_RATE, &bufferFrames, &Audio::RtAudioVorbis, (void *)Audio::SFX);
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

	std::cout << "window x : " << GLHelper::SCR_WIDTH;
	std::cout << "window y : " << GLHelper::SCR_HEIGHT;

	GLHelper::window = glfwCreateWindow(GLHelper::SCR_WIDTH, GLHelper::SCR_HEIGHT, "LODE RUNNER 2020 - Margitai Peter", NULL, NULL);
	glfwSetWindowPosCallback(GLHelper::window, GLHelper::window_pos_callback);
	glfwSetWindowPos(GLHelper::window, GLHelper::windowPosX, GLHelper::windowPosY);

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

	glViewport(GLHelper::viewPortX, GLHelper::viewPortY, GLHelper::viewPortWidth, GLHelper::viewPortHeight);

	GLFWimage icon;
	int iconNrComp;
	icon.pixels = GLHelper::getRawCharArrayWithSTBI("Texture/Runner.png", &icon.width, &icon.height, &iconNrComp, 4);
	glfwSetWindowIcon(GLHelper::window, 1, &icon);

	GLHelper::selectShader = new Shader("Shaders/select_VS.txt", "Shaders/select_FS.txt");
	GLHelper::mainShader = new Shader("Shaders/main_VS.txt", "Shaders/main_FS.txt");
	GLHelper::levelShader = new Shader("Shaders/level_VS.txt", "Shaders/level_FS.txt");
	GLHelper::playerShader = new Shader("Shaders/player_VS.txt", "Shaders/player_FS.txt");

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenVertexArrays(1, &levelVAO);
	glGenBuffers(1, &levelVBO);
	glGenBuffers(1, &levelEBO);

	glBindVertexArray(levelVAO);

	glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, levelEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenVertexArrays(1, &playerVAO);
	glGenBuffers(1, &playerVBO);
	glGenBuffers(1, &playerEBO);

	glBindVertexArray(playerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, playerVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenVertexArrays(1, &mainVAO);
	glGenBuffers(1, &mainVBO);

	glBindVertexArray(mainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(main_menu), main_menu, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glGenVertexArrays(1, &selectVAO);
	glGenBuffers(1, &selectVBO);
	glGenBuffers(1, &selectEBO);

	glBindVertexArray(selectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, selectVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selectEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	float screenRatio = ((float)GLHelper::SCR_WIDTH) / GLHelper::SCR_HEIGHT;

	GLHelper::playerShader->use();
	glActiveTexture(GL_TEXTURE0);
	unsigned int characterSet = GLHelper::loadTexture("Texture/NES - Lode Runner - Characters.png");
	glBindTexture(GL_TEXTURE_2D, characterSet);
	GLHelper::playerShader->setInt("textureA", 0);
	GLHelper::playerShader->setFloat("screenRatio", screenRatio);

	GLHelper::levelShader->use();
	glActiveTexture(GL_TEXTURE1);
	unsigned int tileSet = GLHelper::loadTexture("Texture/NES - Lode Runner - Tileset.png");
	glBindTexture(GL_TEXTURE_2D, tileSet);
	GLHelper::levelShader->setInt("textureA", 1);
	GLHelper::levelShader->setFloat("screenRatio", screenRatio);

	std::string mainMenuTextureName = "Texture/MainMenu.png";
	
	if (championShip)
		mainMenuTextureName = "Texture/Championship.png";
	else if (usCover) 
		mainMenuTextureName = "Texture/MainMenuU.png";
	
	const char *mainMenuTextureNameChar = mainMenuTextureName.c_str();

	GLHelper::mainShader->use();
	glActiveTexture(GL_TEXTURE2);
	unsigned int menuScreen = GLHelper::loadTexture(mainMenuTextureNameChar);
	glBindTexture(GL_TEXTURE_2D, menuScreen);
	GLHelper::mainShader->setInt("textureA", 2);
	GLHelper::mainShader->setInt("width", GLHelper::SCR_WIDTH);
	GLHelper::mainShader->setInt("height", GLHelper::SCR_HEIGHT);
	GLHelper::mainShader->setFloat("screenRatio", screenRatio);

	GLHelper::selectShader->use();
	glActiveTexture(GL_TEXTURE3);
	unsigned int selectScreenT = GLHelper::loadTexture("Texture/ABC.png");
	glBindTexture(GL_TEXTURE_2D, selectScreenT);
	GLHelper::selectShader->setInt("textureA", 3);
	GLHelper::selectShader->setFloat("screenRatio", screenRatio);

	glGenTextures(1, &pauseScreenT);

	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &cursorVAO);
	glGenBuffers(1, &cursorVBO);
	glGenBuffers(1, &cursorEBO);

	glBindVertexArray(cursorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
	float cursorMapping[] = {21.0f/22, 0, 1, 0, 1, 1.0f / 12, 21.0f / 22, 1.0f / 12 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 8, sizeof(float) * 8, cursorMapping);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cursorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));

#ifdef VIDEO_RECORDING
	AudioParameters* audioIn = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);
	AudioParameters* audioOut = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);

	VideoParameters* videoIn = new VideoParameters(GLHelper::viewPortWidth, GLHelper::viewPortHeight, AV_CODEC_ID_NONE, 400000, AV_PIX_FMT_RGB24, STREAM_FRAME_RATE);
	VideoParameters* videoOut = new VideoParameters(0, recordingHeight, AV_CODEC_ID_H264, 400000, AV_PIX_FMT_YUV420P, STREAM_FRAME_RATE);
	
	MultiMedia media(audioIn, audioOut, videoIn, videoOut);
	media.setGLViewPortReferences(&GLHelper::viewPortX, &GLHelper::viewPortY, &GLHelper::viewPortWidth, &GLHelper::viewPortHeight);
	media.setGenerateName(GLHelper::generateNewVideoName);
	media.setVideoOutputSizeWanted(0, recordingHeight);
	Audio::multiMedia = &media;
	GLHelper::multiMedia = &media;
#endif
	std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();
	
	while (!glfwWindowShouldClose(GLHelper::window)) {
		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - prevFrameStart;

		if (work_time.count() < 1000.0f/FPS) {
			std::chrono::duration<double, std::milli> delta_ms(1000.0f/FPS - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		prevFrameStart = std::chrono::system_clock::now();		

		float currentFrame = glfwGetTime();

		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Writing gametime
		if (menu == L04 || menu == C04 || menu == L06 || menu == C06) {
			std::string timeValue = std::to_string(gameTime);
			timeValue = timeValue.substr(0,timeValue.length()-5);
			TextWriting("GAMETIME: " + timeValue + " SEC", -5, 0);
		}
		
		//Playerlocation
		//if (menu == L04 || menu == C04 || menu == L06 || menu == C06) {
			//std::string timeString = "POS X: " + std::to_string(Enemy::enemies[0].Pos.x) + " Y: " + std::to_string(Enemy::enemies[0].Pos.y);
			//TextWriting(timeString, -5, 0);
		//}

		processInput(GLHelper::window);

		if (lAlt.continous() && enter.simple())
			GLHelper::FullscreenSwitch();

		switch (menu) {
			//mainmenu
			case L01:
				mainMenu(currentFrame);
				break;

			case C01:
				break;

			//intro
			case L02:
				introScreen(currentFrame);
				break;

			case C02:
				break;

			//level select
			case L03:
				selectScreen(currentFrame);
				break;

			case C03:
				break;

			// gameplay
			case L04:

				gameScreen(currentFrame);
				break;
			
			case C04:	
				break;

			//outro
			case L05:
				outroScreen(currentFrame);
				break;

			case C05:
				break;

			//pausing
			case L06:
				pauseScreen(currentFrame);
				break;

			//exiting pause
			case C06:
				pauseScreenOut(currentFrame);
				break;

			//gameOver
			case L07:
				gameOverScreen(currentFrame);
				break;

			case C07:
				break;

			//levelGenerator
			case L08:
				generatorScreen(currentFrame);
				break;

			case C08:
				break;
		}

		//take a screenshot
		if (pButton.simple())
			GLHelper::screenCapture();

#ifdef VIDEO_RECORDING
		//With the help of this function you can record videos
		media.recordAndControl(REC.simple());
#endif

		glfwSwapBuffers(GLHelper::window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &levelVAO);
	glDeleteBuffers(1, &levelVBO);
	glDeleteBuffers(1, &levelEBO);
	
	glDeleteVertexArrays(1, &playerVAO);
	glDeleteBuffers(1, &playerVBO);
	glDeleteBuffers(1, &playerEBO);

	glDeleteVertexArrays(1, &cursorVAO);
	glDeleteBuffers(1, &cursorVBO);
	glDeleteBuffers(1, &cursorEBO);

	glDeleteVertexArrays(1, &mainVAO);
	glDeleteBuffers(1, &mainVBO);
	glDeleteBuffers(1, &mainEBO);
	
	glDeleteVertexArrays(1, &selectVAO);
	glDeleteBuffers(1, &selectVBO);
	glDeleteBuffers(1, &selectEBO);

	glDeleteTextures(1, &menuScreen);
	glDeleteTextures(1, &characterSet);
	glDeleteTextures(1, &tileSet);
	glDeleteTextures(1, &selectScreenT);

	delete GLHelper::levelShader;
	delete GLHelper::mainShader;
	delete GLHelper::playerShader;
	delete GLHelper::selectShader;

	glfwTerminate();
	return 0;
}