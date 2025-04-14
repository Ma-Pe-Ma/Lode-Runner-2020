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

#include <json.hpp>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "GameConfiguration.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#ifdef VIDEO_RECORDING
#include "audio/AudioContext.h"
#endif

void GlfwIOContext::initFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	processInput();
}

void GlfwIOContext::finalizeFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void GlfwIOContext::processInput() {
	buttonInputs.config.detect(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);
	buttonInputs.pause.detect(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
#ifndef __EMSCRIPTEN__
	GLFWgamepadstate state;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);

	buttonInputs.left.detect(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.5);
	buttonInputs.right.detect(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.5);
	buttonInputs.up.detect(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.5);
	buttonInputs.down.detect(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.5);
	buttonInputs.rightDig.detect(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0);
	buttonInputs.leftDig.detect(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0);
	buttonInputs.enter.detect(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS);
	buttonInputs.select.detect(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS);

	buttonInputs.screenshot.detect(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
	lAlt.detect(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
#endif

#ifdef VIDEO_RECORDING
	buttonInputs.videoButton.detect(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
#endif

	if (lAlt.continuous() && buttonInputs.enter.simple()) {
		fullscreenSwitch();
	}

#ifndef NDEBUG
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	buttonInputs.d1Left.detect(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);
	buttonInputs.d1Down.detect(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS);
	buttonInputs.d1Right.detect(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS);
	buttonInputs.d1Up.detect(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);

	buttonInputs.d2Up.detect(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS);
	buttonInputs.d2Left.detect(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS);
	buttonInputs.d2Down.detect(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS);
	buttonInputs.d2Right.detect(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS);
#endif
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
	unsigned char* buffer = new unsigned char[std::get<0>(screenParameters.screenSize) * std::get<1>(screenParameters.screenSize) * 3];
	glReadPixels(0, 0, std::get<0>(screenParameters.screenSize), std::get<1>(screenParameters.screenSize), GL_RGB, GL_UNSIGNED_BYTE, buffer);

	std::thread saveThread(&GlfwIOContext::saveImage, this, buffer);
	saveThread.detach();
}

void GlfwIOContext::saveImage(unsigned char* buffer)
{
	std::cout << "Trying to save image..." << std::endl;
	//find next non-existing screenshot identifier
	unsigned int scr = this->findScreenShotCount();

	std::string sname = "resources/screenshots/Screenshot-" + std::to_string(scr) + ".png";

	const char* name = sname.c_str();

	stbi_flip_vertically_on_write(true);

	if (!stbi_write_png(name, std::get<0>(screenParameters.screenSize), std::get<1>(screenParameters.screenSize), 3, buffer, 0)) {
		std::cout << "ERROR: Could not write screenshot file: " << name << std::endl;
	}
	else {
		std::cout << "Screenshot taken as: " << name << std::endl;
	}

	delete[] buffer;
}

unsigned int GlfwIOContext::findVideoCount() {
	int videoCount = 0;

	while (true) {
		std::string stringName = "resources/gameplay-videos/GameplayVideo-" + std::to_string(videoCount) + ".mkv";

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
	return "resources/gameplay-videos/GameplayVideo-" + std::to_string(vid) + ".mkv";
}

unsigned int GlfwIOContext::findScreenShotCount() {
	int screenshotCount = 0;

	while (true) {
		std::string stringName = "resources/screenshots/Screenshot-" + std::to_string(screenshotCount) + ".png";

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
	std::ifstream config("resources/config.txt", std::fstream::in);
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
		this->screenParameters.screenSize = std::make_tuple(1500, 900);
		break;
	case 1:
		this->screenParameters.screenSize = std::make_tuple(750, 450);
		break;
	case 2:
		this->screenParameters.screenSize = std::make_tuple(3000, 1800);
		break;
	case 3:
		this->screenParameters.screenSize = std::make_tuple(1750, 1050);
		break;
	case 4:
		this->screenParameters.screenSize = std::make_tuple(getIntByKey("width", std::get<0>(this->screenParameters.screenSize)), getIntByKey("height", std::get<0>(this->screenParameters.screenSize)));
	}

	std::get<0>(this->screenParameters.screenSize) = std::get<0>(this->screenParameters.screenSize) > 4096 ? 4096 : std::get<0>(this->screenParameters.screenSize);
	std::get<1>(this->screenParameters.screenSize) = std::get<1>(this->screenParameters.screenSize) > 2160 ? 2160 : std::get<1>(this->screenParameters.screenSize);

	gameConfiguration->setPlayerSpeed(getFloatByKey("playerSpeed", 0.9f));
	gameConfiguration->setEnemySpeed(getFloatByKey("enemySpeed", 0.415f));
	gameConfiguration->setGameVersion(getIntByKey("levelset", 0));

	gameConfiguration->setRecordingHeight(getIntByKey("RecordingHeight", 800));
	gameConfiguration->setLevel(0, getIntByKey("levelNr", 1));
	gameConfiguration->setLevel(1, getIntByKey("levelNr", 1));
	gameConfiguration->setFramesPerSec(getIntByKey("FPS", 60));

	this->gameConfiguration = gameConfiguration;

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
		this->screenParameters.formerScreenSize = this->screenParameters.screenSize;
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);		
	}
	else {
		glfwSetWindowMonitor(window, NULL, 0, 0, std::get<0>(this->screenParameters.formerScreenSize), std::get<0>(this->screenParameters.formerScreenSize), 0);
		glfwSetWindowPos(window, std::get<0>(this->screenParameters.screenPosition), std::get<1>(this->screenParameters.screenPosition));
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
	buttonInputs.leftDig.setImpulseTime(0.25f);
	buttonInputs.rightDig.setImpulseTime(0.25f);

	screenParameters.updateViewPortValues(std::get<0>(this->screenParameters.screenSize), std::get<1>(this->screenParameters.screenSize));

	glfwInit();

	glfwSetErrorCallback([](int error, const char* description) -> void {
		std::cout << "GFLW error: " << error << ", description: " << description << std::endl;
		std::cout << std::hex << "Hex: 0x" << error << std::dec << std::endl;
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

	window = glfwCreateWindow(std::get<0>(this->screenParameters.screenSize), std::get<1>(this->screenParameters.screenSize), reinterpret_cast<const char*>(u8"Lode Runner 2020 - Margitai Péter Máté"), NULL, NULL);

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
			self->screenParameters.screenPosition = std::make_tuple(x, y);
		}
	});

	glfwSetWindowPos(window, std::get<0>(this->screenParameters.screenPosition), std::get<1>(this->screenParameters.screenPosition));

	GLFWimage icon;
	int iconNrComp;
	icon.pixels = stbi_load("assets/textures/Runner.png", &icon.width, &icon.height, &iconNrComp, 4);
	glfwSetWindowIcon(window, 1, &icon);

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		//std::cout << "Updating viewport: " << width << ", " << height << std::endl;
		auto self = static_cast<GlfwIOContext*>(glfwGetWindowUserPointer(window));
		self->screenParameters.updateViewPortValues(width, height);

#ifdef VIDEO_RECORDING
		if (self->multiMedia != nullptr) {
			self->multiMedia = nullptr;
		}
#endif // VIDEO_RECORDING

		glViewport(std::get<0>(self->screenParameters.viewPortPosition), std::get<1>(self->screenParameters.viewPortPosition), std::get<0>(self->screenParameters.viewPortSize), std::get<1>(self->screenParameters.viewPortSize));
	});

	glfwSetWindowSizeCallback(window, [](GLFWwindow*, int width, int height) -> void {
		//std::cout << "WINDOW Resized: " << width << ", height: " << height << std::endl;
	});

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

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("./assets/Roboto-Medium.ttf", 13, nullptr);
	io.Fonts->Build();

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
#ifdef VIDEO_RECORDING
	if (multiMedia != nullptr) {
		audio->setMultiMedia(nullptr);
		multiMedia = nullptr;
	}
#endif 
	this->gameConfiguration = nullptr;
	glfwTerminate();
}

void GlfwIOContext::handleScreenRecording()
{
	//take a screenshot
	if (buttonInputs.screenshot.simple()) {
		takeScreenShot();
	}

#ifdef VIDEO_RECORDING
	if (buttonInputs.videoButton.simple()) {
		if (multiMedia == nullptr) {
			multiMedia = initializeMultimedia();
			audio->setMultiMedia(multiMedia);
		}
		else {
			audio->setMultiMedia(nullptr);
			multiMedia = nullptr;			
		}
	}

	if (multiMedia != nullptr) {
		multiMedia->writeVideoFrame();
	}	
#endif
}

void GlfwIOContext::saveConfig(std::string modifiableKey, std::string modifiableValue)
{
	std::string line;

	std::ifstream configFileOld;
	std::ofstream configFileNew;
	configFileOld.open("resources/config.txt");
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

	remove("resources/config.txt");
	rename("config_temp.txt", "resources/config.txt");
}

std::array<std::array<char, 28>, 16> GlfwIOContext::loadLevel(std::string fileName, short levelNumber)
{
	if (!levels.count(fileName)) {
 		std::ifstream f(fileName);
		nlohmann::json data = nlohmann::json::parse(f);

		levels[fileName] = {};

		for (auto levelIt = data.begin(); levelIt != data.end(); levelIt++) {
			short key = std::stoi(levelIt.key());

			for (auto lineIt = levelIt->begin(); lineIt != levelIt->end(); lineIt++) {
				int index = lineIt - levelIt->begin();

				std::array<char, 28> lineArray;

				std::string line = lineIt.value().get<std::string>();
				std::copy(line.begin(), line.begin() + line.size(), lineArray.begin());

				levels[fileName][key][index] = lineArray;

			}
		}

		f.close();
	}

	return levels[fileName][levelNumber - 1];
}

nlohmann::json GlfwIOContext::loadGeneratorLevels() {
	std::ifstream fileStream;
	fileStream.open(generatorFilePath);

	if (fileStream.fail()) {
		std::cout << "Generator not exist!" << std::endl;
		std::ofstream newFile;
		newFile.open(generatorFilePath);
		newFile << "{}";
	}

	fileStream.open(generatorFilePath);

	return nlohmann::json::parse(fileStream);
}

void GlfwIOContext::saveGeneratorLevels(nlohmann::json generatorLevels) {
	std::ofstream outFile(generatorFilePath);
	outFile << generatorLevels.dump(2);
	outFile.close();
}

#ifdef VIDEO_RECORDING

#define STREAM_FRAME_RATE 60
#define STREAM_BIT_RATE 400000

std::shared_ptr<MultiMedia> GlfwIOContext::initializeMultimedia() {
	AudioParameters* audioIn = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);
	AudioParameters* audioOut = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);

	unsigned int recordingHeight = gameConfiguration->getRecordingHeight();
	unsigned int recordingWidth = unsigned int(recordingHeight * 16.0f / 9);

	std::tuple<unsigned int, unsigned int> outputSize = MultiMedia::determineOutput(std::get<0>(screenParameters.viewPortSize), std::get<1>(screenParameters.viewPortSize), recordingWidth, recordingHeight);

	VideoParameters* videoIn = new VideoParameters(std::get<0>(screenParameters.viewPortSize), std::get<1>(screenParameters.viewPortSize), STREAM_BIT_RATE, gameConfiguration->getFramesPerSec(), AV_CODEC_ID_NONE, AV_PIX_FMT_RGB24);
	VideoParameters* videoOut = new VideoParameters(std::get<0>(outputSize), std::get<1>(outputSize), STREAM_BIT_RATE, gameConfiguration->getFramesPerSec(), AV_CODEC_ID_H264, AV_PIX_FMT_YUV420P);
	
	return std::make_shared<MultiMedia>(generateNewVideoName(), audioIn, audioOut, videoIn, videoOut,
		[this](unsigned char* buffer) -> void {
			glReadPixels(std::get<0>(screenParameters.viewPortPosition), std::get<1>(screenParameters.viewPortPosition), std::get<0>(screenParameters.viewPortSize), std::get<1>(screenParameters.viewPortSize), GL_RGB, GL_UNSIGNED_BYTE, buffer);
		}
	);
}
#endif