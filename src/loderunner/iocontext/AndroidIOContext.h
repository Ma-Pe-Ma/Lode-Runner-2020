#ifndef ANDROIDIOCONTEXT_H
#define ANDROIDIOCONTEXT_H

#include <IOContext.h>

class AndroidIOContext : public IOContext {

public:

	void saveConfig(std::string key, std::string value) override {
		ndk_helper::JNIHelper* jniHelper = ndk_helper::JNIHelper::GetInstance();
		jniHelper->setLastLevel(stateContext->level[stateContext->playerNr]);
	}

	extern "C" {
		JNIEXPORT void JNICALL
			Java_com_mpm_lodeRunner_GameActivity_processInput(JNIEnv* env, jobject thiz, jint buttonID, jboolean pushed) {
			switch ((int)buttonID) {
			case 0:
				up.detectAlter((bool)pushed);
				break;
			case 1:
				rightButton.detectAlter((bool)pushed);
				break;
			case 2:
				down.detectAlter((bool)pushed);
				break;
			case 3:
				leftButton.detectAlter((bool)pushed);
				break;
			case 4:
				leftDigButton.detectAlter((bool)pushed);
				break;
			case 5:
				rightDigButton.detectAlter((bool)pushed);
				break;
			case 6:
				space.detectAlter((bool)pushed);
				break;
			case 7:
				enter.detectAlter((bool)pushed);
				break;
			}
		}

		JNIEXPORT void JNICALL
			Java_com_mpm_lodeRunner_GameActivity_initializeGame(JNIEnv* env, jobject thiz, jboolean champ, jboolean ntscCover, jboolean joystick, jint levelIn, jfloat player, jfloat enemy) {
			gameVersion = 0;

			if ((bool)champ) {
				gameVersion = 1;
				levelFileName = "Level/ChampionshipLevels.txt";
			}

			usCover = (bool)ntscCover;
			startingLevel = (unsigned int)levelIn;

			Enemy::setPlayerSpeed((float)player);
			Enemy::setEnemySpeed((float)enemy);
		}

		JNIEXPORT void JNICALL
			Java_com_mpm_lodeRunner_GameActivity_processJoystick(JNIEnv* env, jobject thiz, jint jangle, jint jstrength) {
			int angle = (int)jangle;
			int strength = (int)jstrength;

			const int threshold = 30;
			const int outerAngle = 20;

			if (strength > threshold) {
				rightButton.detectAlter(angle < 45 + outerAngle || angle > 315 - outerAngle);
				up.detectAlter(45 - outerAngle < angle&& angle < 135 + outerAngle);
				leftButton.detectAlter(135 - outerAngle < angle&& angle < 225 + outerAngle);
				down.detectAlter(225 - outerAngle < angle&& angle < 315 + outerAngle);
			}
			else {
				rightButton.detectAlter(false);
				up.detectAlter(false);
				leftButton.detectAlter(false);
				down.detectAlter(false);
			}
		}
	}

	void processInput() override
	{

	}

	void updateViewPort() {
		ndk_helper::GLContext* context = ndk_helper::GLContext::GetInstance();
		updateViewPortValues(context->GetScreenWidth(), context->GetScreenHeight());

		Helper::log("Width: " + std::to_string(context->GetScreenWidth()) + ", height: " + std::to_string(context->GetScreenHeight()));

		glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
	}

	void loadLevel(std::string levelFileName, std::function<bool(std::string)> lineProcessor) override
	{
		std::vector<uint8_t> data;
		if (!ndk_helper::JNIHelper::GetInstance()->ReadFile(levelFileName.c_str(), &data)) {
			Helper::log("Can't read file!" + levelFileName);
			LOGI("Can not open a file:%s", levelFileName.c_str());
			return;
		}

		const GLchar* source = (GLchar*)&data[0];
		std::string test1(source);

		std::stringstream test;
		test << test1;
		std::string row;

		while (std::getline(test, row, '\n')) {
			if (lineProcessor(row))
			{
				break;
			}
		}
	}
};

#endif