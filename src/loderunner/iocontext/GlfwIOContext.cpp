#include "GlfwIOContext.h"

#include <iostream>
#include <thread>
#include <string>
#include <fstream>

#if defined __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <GLES3/gl3.h>
#else 
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "GameConfiguration.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

void GlfwIOContext::processInput() {

	configButton.detect(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);
	pauseButton.detect(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
#ifndef __EMSCRIPTEN__
	GLFWgamepadstate state;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);

	leftButton.detect(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.5);
	rightButton.detect(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.5);
	up.detect(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.5);
	down.detect(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.5);
	rightDigButton.detect(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0);
	leftDigButton.detect(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0);
	enter.detect(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS);
	space.detect(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS);

	pButton.detect(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
	lAlt.detect(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
#endif

#ifdef VIDEO_RECORDING
	REC.detect(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
#endif

	if (lAlt.continuous() && enter.simple()) {
		fullscreenSwitch();
	}

#ifndef NDEBUG
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	fButton.detect(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);
	gButton.detect(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS);
	hButton.detect(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS);
	tButton.detect(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);

	iButton.detect(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS);
	jButton.detect(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS);
	kButton.detect(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS);
	lButton.detect(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS);
#endif
}

void GlfwIOContext::finalizeFrame()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

unsigned int GlfwIOContext::loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

#if !defined __EMSCRIPTEN__		
		float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void GlfwIOContext::takeScreenShot()
{
	unsigned char* buffer = new unsigned char[std::get<0>(screenSize) * std::get<1>(screenSize) * 3];
	glReadPixels(0, 0, std::get<0>(screenSize), std::get<1>(screenSize), GL_RGB, GL_UNSIGNED_BYTE, buffer);

	std::thread saveThread(&GlfwIOContext::saveImage, this, buffer);
	saveThread.detach();
}

void GlfwIOContext::saveImage(unsigned char* buffer)
{
	std::cout << "\n Trying to save image...";
	//find next non-existing screenshot identifier
	unsigned int scr = this->findScreenShotCount();

	std::string sname = "Screenshots/Screenshot-" + std::to_string(scr) + ".png";

	const char* name = sname.c_str();

	stbi_flip_vertically_on_write(true);

	if (!stbi_write_png(name, std::get<0>(screenSize), std::get<1>(screenSize), 3, buffer, 0)) {
		std::cout << "\nERROR: Could not write screenshot file: " << name;
	}
	else {
		std::cout << "\nScreenshot taken as: " << name;
	}

	delete[] buffer;
}

unsigned int GlfwIOContext::findVideoCount() {
	int videoCount = 0;

	while (true) {
		std::string stringName = "GameplayVideos/GameplayVideo-" + std::to_string(videoCount) + ".mkv";

		std::ifstream fileStream;
		fileStream.open(stringName);

		if (fileStream.fail()) {
			fileStream.close();
			return videoCount;
		}
		else {
			videoCount++;
			fileStream.close();
		}
	}

	return 0;
}

std::string GlfwIOContext::generateNewVideoName() {
	unsigned int vid = findVideoCount();
	return "GameplayVideos/GameplayVideo-" + std::to_string(vid) + ".mkv";
}

unsigned int GlfwIOContext::findScreenShotCount() {
	int screenshotCount = 0;

	while (true) {
		std::string stringName = "Screenshots/Screenshot-" + std::to_string(screenshotCount) + ".png";

		std::ifstream fileStream;
		fileStream.open(stringName);

		if (fileStream.fail()) {
			fileStream.close();
			return screenshotCount;
		}
		else {
			screenshotCount++;
			fileStream.close();
		}
	}

	return 0;
}

void GlfwIOContext::loadConfig(std::shared_ptr<GameConfiguration> gameConfiguration) {
	std::ifstream config("config.txt", std::fstream::in);
	std::string line;

	if (config.is_open()) {
		while (getline(config, line)) {
			if (line.length() == 0 || line[0] == '#') {
				continue;
			}

			std::string key = line.substr(0, line.find(' '));
			std::string value = line.substr(line.find(' ') + 1);

			configMap[key] = value;
		}
	}
	else {
		std::cout << "\n Config File Not Found!";
	}

	int resolutionMode = getIntByKey("resolution", 0);

	switch (resolutionMode) {
	case 0:
		this->screenSize = std::make_tuple(1500, 900);
		break;
	case 1:
		this->screenSize = std::make_tuple(750, 450);
		break;
	case 2:
		this->screenSize = std::make_tuple(3000, 1800);
		break;
	case 3:
		this->screenSize = std::make_tuple(1750, 1050);
		break;
	case 4:
		this->screenSize = std::make_tuple(getIntByKey("width", std::get<0>(this->screenSize)), getIntByKey("height", std::get<0>(this->screenSize)));
	}

	std::get<0>(screenSize) = std::get<0>(screenSize) > 4096 ? 4096 : std::get<0>(screenSize);
	std::get<1>(screenSize) = std::get<1>(screenSize) > 2160 ? 2160 : std::get<1>(screenSize);

	gameConfiguration->setPlayerSpeed(getFloatByKey("playerSpeed", 0.9f));
	gameConfiguration->setEnemySpeed(getFloatByKey("enemySpeed", 0.415f));
	gameConfiguration->setGameVersion(getIntByKey("levelset", 0));

	gameConfiguration->setRecordingHeight(getIntByKey("RecordingHeight", 800));
	gameConfiguration->setStartingLevel(getIntByKey("levelNr", 1));
	gameConfiguration->setFramesPerSec(getIntByKey("FPS", 60));

	config.close();
}

int GlfwIOContext::getIntByKey(std::string key, int defaultValue) {
	std::string value = configMap[key];

	if (value != "") {
		try {
			return std::stoi(value);
		}
		catch (std::invalid_argument& e) {

		}
		catch (std::out_of_range& e) {
			
		}		
	}

	return defaultValue;
}

float GlfwIOContext::getFloatByKey(std::string key, float defaultValue) {
	std::string value = configMap[key];

	if (value != "") {
		try {
			return std::stof(value);
		}
		catch (std::invalid_argument e) {

		}
	}

	return defaultValue;
}

void GlfwIOContext::fullscreenSwitch() {
	if (!fullScreen) {
		formerScreenSize = screenSize;
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);		
	}
	else {
		glfwSetWindowMonitor(window, NULL, 0, 0, std::get<0>(formerScreenSize), std::get<0>(formerScreenSize), 0);
		glfwSetWindowPos(window, std::get<0>(screenPosition), std::get<1>(screenPosition));
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	fullScreen = !fullScreen;

	glfwMakeContextCurrent(window);
}

bool GlfwIOContext::shouldClose()
{
	return !glfwWindowShouldClose(window);
}

void GlfwIOContext::initialize()
{
	leftDigButton.setImpulseTime(0.25f);
	rightDigButton.setImpulseTime(0.25f);

	updateViewPortValues(std::get<0>(screenSize), std::get<1>(screenSize));

	glfwInit();

	glfwSetErrorCallback([](int error, const char* description) -> void {
		std::cout << "\nGFLW error: " << error << ", description: " << description;
		std::cout << std::hex << "\n hex: 0x" << error;
		std::cout << std::dec;
	});

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

	window = glfwCreateWindow(std::get<0>(screenSize), std::get<1>(screenSize), reinterpret_cast<const char*>(u8"Lode Runner 2020 - Margitai Péter Máté"), NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		int a;
		std::cin >> a;
		return;
	}
	
	glfwSetWindowUserPointer(window, static_cast<void*>(this));
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetWindowPosCallback(window, [](GLFWwindow* window, int x, int y) {
		auto self = static_cast<GlfwIOContext*>(glfwGetWindowUserPointer(window));

		if (self->fullScreen) {
			self->screenPosition = std::make_tuple(x, y);
		}
	});

	glfwSetWindowPos(window, std::get<0>(screenPosition), std::get<1>(screenPosition));

	GLFWimage icon;
	int iconNrComp;
	icon.pixels = stbi_load("Assets/Texture/Runner.png", &icon.width, &icon.height, &iconNrComp, 4);
	glfwSetWindowIcon(window, 1, &icon);

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		//std::cout << "\n updating viewport: " << width << ", " << height;
		auto self = static_cast<GlfwIOContext*>(glfwGetWindowUserPointer(window));

		self->updateViewPortValues(width, height);

#ifdef VIDEO_RECORDING
		if (self->multiMedia != nullptr) {
			self->windowResized();
		}
#endif // VIDEO_RECORDING

		glViewport(std::get<0>(self->getViewPortPosition()), std::get<1>(self->getViewPortPosition()), std::get<0>(self->getViewPortSize()), std::get<1>(self->getViewPortSize()));
	});

	glfwSetWindowSizeCallback(window, [](GLFWwindow*, int width, int height) -> void {
		//std::cout << "WINDOW Resized: " << width << ", height: " << height << std::endl;
	});

	glfwSetWindowSize(window, std::get<0>(screenSize), std::get<1>(screenSize));

#if !defined __EMSCRIPTEN__
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		int a;
		std::cin >> a;
		return;
	}
#endif

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//int major, minor, rev;
	//glfwGetVersion(&major, &minor, &rev);
	//std::cout << "GLFW - major: " << major << ", minor: " << minor << ", rev: " << rev << std::endl;

	ImGui_ImplGlfw_InitForOpenGL(window, true);

#ifdef __EMSCRIPTEN__
	ImGui_ImplOpenGL3_Init("#version 300 es");
#else
	ImGui_ImplOpenGL3_Init("#version 460 core");
#endif

	ImGui::StyleColorsDark();
}

void GlfwIOContext::terminate()
{
	glfwTerminate();
}

void GlfwIOContext::handleScreenRecording()
{
	//take a screenshot
	if (pButton.simple()) {
		takeScreenShot();
	}

#ifdef VIDEO_RECORDING
	//With the help of this function you can record videos
	media->recordAndControl(REC.simple());
#endif
}

void GlfwIOContext::saveConfig(std::string modifiableKey, std::string modifiableValue)
{
	std::string line;

	std::ifstream configFileOld;
	std::ofstream configFileNew;
	configFileOld.open("config.txt");
	configFileNew.open("config_temp.txt");
	bool modifialbleLineFound = false;

	while (getline(configFileOld, line)) {
		if (line.length() == 0 || line[0] == '#') {
			configFileNew << line + "\n";
			continue;
		}

		std::string lineKey = line.substr(0, line.find(' '));
		std::string lineValue = line.substr(line.find(' ') + 1);

		if (lineKey == modifiableKey) {
			modifialbleLineFound = true;

			std::string newLine = modifiableKey + " " + modifiableValue;
			configFileNew << newLine + "\n";
}
		else {
			configFileNew << line + "\n";
		}
	}

	if (!modifialbleLineFound) {
		std::string newLine = modifiableKey + " " + modifiableValue;
		configFileNew << newLine + "\n";
	}

	configFileNew.close();
	configFileOld.close();

	remove("config.txt");
	rename("config_temp.txt", "config.txt");
}

void GlfwIOContext::loadLevel(std::string fileName, std::function<bool(std::string)> lineProcessor)
{
	std::string row;

	std::fstream levelFile;
	levelFile.open(fileName);
	while (std::getline(levelFile, row, '\n')) {
		if (lineProcessor(row))
		{
			break;
		}
	}

	levelFile.close();
}

#ifdef VIDEO_RECORDING
void GlfwIOContext::initializeMultimedia()
{
	AudioParameters* audioIn = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);
	AudioParameters* audioOut = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);

	VideoParameters* videoIn = new VideoParameters(GLHelper::viewPortWidth, GLHelper::viewPortHeight, AV_CODEC_ID_NONE, 400000, AV_PIX_FMT_RGB24, STREAM_FRAME_RATE);
	VideoParameters* videoOut = new VideoParameters(0, recordingHeight, AV_CODEC_ID_H264, 400000, AV_PIX_FMT_YUV420P, STREAM_FRAME_RATE);

	media = std::make_shared<MultiMedia>(audioIn, audioOut, videoIn, videoOut);

	media->setGLViewPortReferences(&GLHelper::viewPortX, &GLHelper::viewPortY, &GLHelper::viewPortWidth, &GLHelper::viewPortHeight);
	media->setGenerateName(generateNewVideoName);
	media->setVideoOutputSizeWanted(0, recordingHeight);
}
#endif