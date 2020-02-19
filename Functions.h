#include <bitset>

void FindScreenShotCount() {
	while (true) {
		std::string stringName = "Screenshots/Screenshot-" + std::to_string(scr) + ".png";

		std::ifstream fileStream;
		fileStream.open(stringName);

		if (fileStream.fail()) {
			std::cout << "\n scr: " << scr;
			fileStream.close();
			break;
		}
		else {
			scr++;
			fileStream.close();
		}
	}
}

void FindVideoCount() {
	while (true) {
		std::string stringName = "GameplayVideos/GameplayVideo-" + std::to_string(vid) + ".mkv";

		std::ifstream fileStream;
		fileStream.open(stringName);

		if (fileStream.fail()) {
			std::cout << "\n vid: " << vid;
			fileStream.close();
			break;
		}
		else {
			vid++;
			fileStream.close();
		}
	}
}

void UpdateViewPortValues(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	viewPortWidth = SCR_WIDTH;
	viewPortHeight = SCR_HEIGHT;

	if (viewPortHeight % 2 == 1)
		viewPortHeight--;

	if (viewPortWidth % 2 == 1)
		viewPortWidth--;

	viewPortX = (width - 15.0f / 9 * height) / 2;
	viewPortY = (height - 9.0f / 15 * width) / 2;

	float screenRatio = ((float)width) / height;

	if (screenRatio >= 15.0f / 9) {

		viewPortY = 0;
		viewPortHeight = height;

		if (viewPortHeight % 2 == 1)
			viewPortHeight--;

		viewPortWidth = 15.0f / 9 * height;

		if (viewPortWidth % 2 == 1)
			viewPortWidth--;

		viewPortX = (width - viewPortWidth) / 2;
	}
	else {
		viewPortX = 0;
		viewPortWidth = width;

		if (viewPortWidth % 2 == 1)
			viewPortWidth--;

		viewPortHeight = 9.0f / 15 * viewPortWidth;

		if (viewPortHeight % 2 == 1)
			viewPortHeight--;

		viewPortY = (height - viewPortHeight) / 2;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	
	UpdateViewPortValues(width, height);
	
	if(recordingState == recording)
		recordingState = closing;

	glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
}



//Taking Screenshot
void screenCapture() {
	unsigned char *buffer = new unsigned char[SCR_WIDTH * SCR_HEIGHT * 3];
	glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	//find next non-existing screenshot identifier
	FindScreenShotCount();

	std::string sname = "Screenshots/Screenshot-" + std::to_string(scr) + ".png";				//ide megadni hogy hogyan nevezze el a screenshotokat...
	scr++;

	const char *name = sname.c_str();

	stbi_flip_vertically_on_write(true);

	if (!stbi_write_png(name, SCR_WIDTH, SCR_HEIGHT, 3, buffer, 0)) {
		std::cout << "\nERROR: Could not write screenshot file: " << name;
	}
	else std::cout << "\nScreenshot taken as: " << name;
	
	delete[] buffer;
}

unsigned int loadTexture(char const * path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		
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

//from left to right, from down to up
void playerTextureMapping(int index) {

	blokk[8] = ((index % 12) * 1.0) / 12;
	blokk[9] = ((index / 12) * 1.0) / 5;

	blokk[10] = ((index % 12 + 1) * 1.0) / 12;
	blokk[11] = ((index / 12) * 1.0) / 5;

	blokk[12] = ((index % 12 + 1) * 1.0) / 12;
	blokk[13] = ((index / 12 + 1) * 1.0) / 5;

	blokk[14] = ((index % 12) * 1.0) / 12;
	blokk[15] = ((index / 12 + 1) * 1.0) / 5;
}

//from up to down, from left to right
void levelTextureMapping(int index) {

	blokk[8] = ((index / 6) * 1.0) / 11;
	blokk[9] = 5.0 / 6 - ((index % 6) * 1.0) / 6;

	blokk[10] = ((index / 6 + 1) * 1.0) / 11;
	blokk[11] = 5.0 / 6 - ((index % 6) * 1.0) / 6;

	blokk[12] = ((index / 6 + 1)*1.0) / 11;
	blokk[13] = 1.0 - ((index % 6)*1.0) / 6;

	blokk[14] = ((index / 6)*1.0) / 11;
	blokk[15] = 1.0 - ((index % 6)*1.0) / 6;
}

void abcTextureMapping(char karakter) {
	std::string ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (karakter == '.') {

		blokk[8] = float(0) / 16;
		blokk[9] = (3.0f) / 4;

		blokk[10] = float(1) / 16;
		blokk[11] = (3.0f) / 4;

		blokk[12] = float(1) / 16;
		blokk[13] = (4.0f) / 4;

		blokk[14] = float(0) / 16;
		blokk[15] = (4.0f) / 4;
	
		return;
	}

	if (karakter == ':') {

		blokk[8] = float(1) / 16;
		blokk[9] = (3.0f) / 4;

		blokk[10] = float(2) / 16;
		blokk[11] = (3.0f) / 4;

		blokk[12] = float(2) / 16;
		blokk[13] = (4.0f) / 4;

		blokk[14] = float(1) / 16;
		blokk[15] = (4.0f) / 4;

		return;
	}

	for (int j = 0; j < 10; j++)
		if (j == (int(karakter) - 48)) {
			blokk[8] = (1.0*j + 0.01) / 16;
			blokk[9] = (1.0) / 2;

			blokk[10] = (j + 1.0) / 16;
			blokk[11] = (1.0) / 2;

			blokk[12] = (j + 1.0) / 16;
			blokk[13] = (3.0) / 4;

			blokk[14] = (1.0*j + 0.01) / 16;
			blokk[15] = (3.0) / 4;
		}

	for (int i = 0; i < ABC.length(); i++) {
		if (karakter == ABC[i]) {
			if (i < 15) {
				blokk[8] = (float(1.01 + i)) / 16;
				blokk[9] = (float(1)) / 4;

				blokk[10] = (float(2 + i)) / 16;
				blokk[11] = (float(1)) / 4;

				blokk[12] = (float(2 + i)) / 16;
				blokk[13] = (float(2)) / 4;

				blokk[14] = (float(1.01 + i)) / 16;
				blokk[15] = (float(2)) / 4;
				return;
			}

			if (i > 14) {
				blokk[8] = float(i - 14.99) / 16;
				blokk[9] = (0.0) / 4;

				blokk[10] = float(i - 14) / 16;
				blokk[11] = (0.0) / 4;

				blokk[12] = float(i - 14) / 16;
				blokk[13] = (1.0) / 4;

				blokk[14] = float(i - 14.99) / 16;
				blokk[15] = (1.0) / 4;
				return;
			}
		}
	}
}

void TextWriting(std::string text, float i, float j) {
	for (int k = 0; k < text.length(); k++) {
		if (i == 32) {
			i = 0;
			j++;
		}

		if (text.at(k) != ' ') {

			abcTextureMapping(text.at(k));
			
			selectShader->use();
			selectShader->setVec2("gPos", glm::vec2(i, j));
			selectShader->setInt("textureA", 3);

			glBindVertexArray(selectVAO);
			glBindBuffer(GL_ARRAY_BUFFER, selectVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(blokk), blokk, GL_STATIC_DRAW);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		i++;
	}
}

void DrawLevel(float x, float y, int ref, float holeTimer) {
		
	if (holeTimer == -2)
		ref = 24;

	levelTextureMapping(ref);

	levelShader->use();
	glBindVertexArray(levelVAO);
	glBindBuffer(GL_ARRAY_BUFFER, levelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blokk), blokk, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, (void*) &blokk[8]);
	levelShader->setVec2("gPos", x, y);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DrawEnemy(float x, float y, int ref, Direction direction, Gold* gold) {
	bool dirToShader = true;

	if (direction == right)
		dirToShader = false;

	bool carryGold = false;

	if (gold != nullptr)
		carryGold = true;

	playerTextureMapping(ref);

	playerShader->use();
	glBindVertexArray(playerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blokk), blokk, GL_STATIC_DRAW);
	playerShader->setVec2("gPos", glm::vec2(x, y));
	playerShader->setBool("direction", dirToShader);
	playerShader->setBool("carryGold", carryGold);
	playerShader->setFloat("ref", blokk[8]);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void processInput(GLFWwindow *window) {
	
	GLFWgamepadstate state;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	speed = seb * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.5)
		leftButton.detect(1);
	else
		leftButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.5)
		rightButton.detect(1);
	else
		rightButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.5)
		up.detect(1);
	else
		up.detect(0);

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.5)
		down.detect(1);
	else
		down.detect(0);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		REC.detect(1);
	else
		REC.detect(0);

	/* (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		debugPos.x = -enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		debugPos.x =  enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		debugPos.y =  enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		debugPos.y = -enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		enemies[2].dPos.x = -enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		enemies[2].dPos.x = +enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		enemies[2].dPos.y = +enemySpeed * speed;

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		enemies[2].dPos.y = -enemySpeed * speed;

	*/
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0)
		rightDigButton.detect(1);
	else
		rightDigButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0)
		leftDigButton.detect(1);
	else
		leftDigButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS)
		enter.detect(1);
	else
		enter.detect(0);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS)
		space.detect(1);
	else
		space.detect(0);

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		pButton.detect(1);
	else
		pButton.detect(0);

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		lAlt.detect(1);
	else
		lAlt.detect(0);
}

static void errorCallback(int error, const char *description) {
	std::cout << "\n error: " << error<<", description: "<<description;
	std::cout << std::hex << "\n hex: 0x" << error;

	std::cout << std::dec;
}

void loadConfig() {
	
	//FPS = STREAM_FRAME_RATE;

	std::ifstream config("config.txt", std::fstream::in);

	std::string setup;

	if (config.is_open()) {
		while (getline(config, setup)) {
			if (setup[0] == '#')
				continue;

			for (int i = 0; i < setup.length(); i++) {
				
				if (setup.compare(0, 5, "width") == 0)
					SCR_WIDTH = std::stoi(setup.substr(6, 4));

				if (setup.compare(0, 6, "height") == 0)
					SCR_HEIGHT = std::stoi(setup.substr(7, 4));

				if (setup.compare(0, 10, "resolution") == 0) {
					if (std::stoi(setup.substr(11, 1)) == 0) {
						SCR_WIDTH = 1500;
						SCR_HEIGHT = 900;
					}

					if (std::stoi(setup.substr(11, 1)) == 1) {
						SCR_WIDTH = 750;
						SCR_HEIGHT = 450;
					}

					if (std::stoi(setup.substr(11, 1)) == 2) {
						SCR_WIDTH = 3000;
						SCR_HEIGHT = 1800;
					}

					if (std::stoi(setup.substr(11, 1)) == 3) {
						SCR_WIDTH = 1750;
						SCR_HEIGHT = 1050;
					}
				}

				if (setup.compare(0, 11, "playerSpeed") == 0)
					playerSpeed = std::stof(setup.substr(12));

				if (setup.compare(0, 10, "enemySpeed") == 0)
					enemySpeed = std::stof(setup.substr(11));

				if (setup.compare(0, 3, "FPS") == 0)
					FPS = std::stoi(setup.substr(4));

				if (setup.compare(0, 8, "levelSet") == 0) {
					if (std::stoi(setup.substr(9)) == 0)
						levelFileName = "level/lodeRunner.baseLevel.txt";

					if (std::stoi(setup.substr(9)) == 1) {
						levelFileName = "level/lodeRunner.champLevel.txt";
						championShip = true;

						if (level[0] > 51) {
							level[0] = 51;
							level[1] = 51;
						}
					}
				}

				if (setup.compare(0, 8, "US COVER") == 0)
					usCover = std::stoi(setup.substr(9));

				if (setup.compare(0, 7, "levelNr") == 0) {
					int levelNr = std::stoi(setup.substr(8));

					if (levelNr < 1) levelNr = 1;
					if (levelNr > 150) levelNr = 150;

					level[0] = levelNr;
					level[1] = levelNr;

					if (championShip)
						if (level[0] > 51) {
							level[0] = 51;
							level[1] = 51;
						}
				}

				if (setup.compare(0, 16, "Recording height") == 0)
					recordinghHeight = std::stoul(setup.substr(17));
			}
			UpdateViewPortValues(SCR_WIDTH, SCR_HEIGHT);
		}

		leftDigButton.setImpulseTime(0.3);
		rightDigButton.setImpulseTime(0.3);
	}
	else
		std::cout << "\n Config File Not Found!";


	FindScreenShotCount();
	FindVideoCount();

	config.close();
}

int RtAudioVorbis(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData) {
	short *out = (short *)outputBuffer;
	Audio *data = (Audio *)userData;

	if (status)
		std::cout << "Stream underflow detected!" << std::endl;

	int s = 0;
	for (int i = 0; i < SOUND_FILE_NR; i++)
		if ((data + i)->GetPlayStatus() == playing) {

			long ret = (data + i)->ReadNextBuffer(pcmout[s]);

			if (ret == 0)
				(data + i)->StopAndRewind();

			s++;
		}

	//if (av_compare_ts(video.next_pts, video.AudioCodecContext->time_base,STREAM_DURATION, (AVRational) { 1, 1 }) >= 0);
	//else;

	for (int i = 0; i < nBufferFrames; i++)
		for (int j = 0; j < CHANNEL_COUNT; j++) {
			short ki = 0;
			for (int k = 0; k < s; k++)
				ki += (((short(pcmout[k][2 * CHANNEL_COUNT*i + 2 * j] << 8)) + pcmout[k][2 * CHANNEL_COUNT * i + 2 * j + 1])) / s;

#ifdef VIDEO_RECORDING
			if (recordingState == recording && GameVideo->Initialized())
				GameVideo->audio->EncodeFrame(ki);
#endif
			*out++ = ki;
		}

	return 0;
}

void FullscreenSwitch() {
	
	if (!fullScreen) {

		const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	}
	else {	

		glfwSetWindowMonitor(window, NULL, 0, 0, SCR_WIDTH, SCR_HEIGHT, 0);
		glfwSetWindowPos(window, 100, 100);
	}

	fullScreen = !fullScreen;

	glfwMakeContextCurrent(window);
}

#ifdef VIDEO_RECORDING

void InitializeGameVideo() {
	
	//not the best method as 
	audioIn = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);
	audioOut = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);
	
	videoIn = new VideoParameters(viewPortWidth, viewPortHeight, AV_CODEC_ID_NONE, 400000, AV_PIX_FMT_RGB24, STREAM_FRAME_RATE);

	unsigned int usedRecordingHeight = viewPortHeight;

	if (viewPortHeight > recordinghHeight)
		usedRecordingHeight = recordinghHeight;

	if (recordinghHeight) {

		if (usedRecordingHeight % 2 == 1)
			usedRecordingHeight--;

		int usedRecordingWidth = (int)((1.0f * viewPortWidth / viewPortHeight) * usedRecordingHeight);

		if (usedRecordingWidth % 2 == 1)
			usedRecordingWidth--;

		videoOut = new VideoParameters(usedRecordingWidth, usedRecordingHeight, AV_CODEC_ID_H264, 400000, AV_PIX_FMT_YUV420P, STREAM_FRAME_RATE);
	}

	else
		videoOut = new VideoParameters(viewPortWidth, viewPortHeight, AV_CODEC_ID_H264, 400000, AV_PIX_FMT_YUV420P, STREAM_FRAME_RATE);


	GameVideo = new MultiMedia(videoOut, videoIn, audioOut, audioIn);

	std::string fileName = "GameplayVideos/GameplayVideo-" + std::to_string(vid) + ".mkv";

	FindVideoCount();

	GameVideo->Initialize(fileName);
	GameVideo->video->SetRecordStartTime(glfwGetTime());
}

void WriteVideoFrame() {
	//std::cout << "\n\n frameNr: " << recordFrameNr++ << ", time: " << diff << ", average fps: " << diff/recordFrameNr<<", "<<recordFrameNr/diff;// << ", fps theo: " << (glfwGetTime() - recordTime) * 50 * 20;
	if (GameVideo->Initialized()) {

		//glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, GameVideo->video->buffer);
		glReadPixels(viewPortX, viewPortY, viewPortWidth, viewPortHeight, GL_RGB, GL_UNSIGNED_BYTE, GameVideo->video->buffer);
		GameVideo->video->EncodeFrame(glfwGetTime());

		while (GameVideo->audio->have || GameVideo->video->have) {

			if (GameVideo->video->have && (!GameVideo->audio->have)) {
				av_interleaved_write_frame(GameVideo->formatContext, GameVideo->video->packet);
				GameVideo->video->have = false;
			}
			else if (GameVideo->audio->have && GameVideo->video->have && av_compare_ts(GameVideo->video->nextPts, GameVideo->video->codecContext->time_base, GameVideo->audio->nextPts, GameVideo->audio->codecContext->time_base) <= 0) {
				av_interleaved_write_frame(GameVideo->formatContext, GameVideo->video->packet);
				GameVideo->video->have = false;
			}
			else {
				av_interleaved_write_frame(GameVideo->formatContext, GameVideo->audio->packet);
				GameVideo->audio->have = false;
			}
		}
	}
}

void CloseGameVideo() {

	GameVideo->CloseVideo();

	delete audioIn;
	delete audioOut;
	delete videoIn;
	delete videoOut;
	delete GameVideo;
}

void RecordHandling() {
	if (recordingState == uninitialized) {

		if (REC.Simple()) {

			InitializeGameVideo();
			recordingState = recording;
		}
	}
	else if (recordingState == recording) {

		if (REC.Simple())
			recordingState = closing;

		WriteVideoFrame();
	}
	else if (recordingState == closing) {

		CloseGameVideo();
		recordingState = uninitialized;
	}
}
#endif