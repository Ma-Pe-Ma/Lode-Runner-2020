#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include <string>
#include <algorithm>

#include "rendering/Text.h"

#include <json.hpp>

#define GAME_VERSION_NUMBER 5

class GameConfiguration {
	float enemySpeed = 0.415f;
	float playerSpeed = 0.9f;

	int gameVersion = 0;
	std::array<int, 2> level = { 1, 1 };
	
	unsigned int recordingHeight = 800;
	unsigned int framesPerSec = 60;	

#ifndef NDEBUG
	bool debugEnemy = false;
#endif

public:
	//game version - {filename, maxlevel, textureID, name, mainmenu texts}
	std::map<int, std::tuple<std::string, short, int, std::string, std::shared_ptr<std::vector<std::shared_ptr<Text>>>>> configurations = {
		{0, {"assets/levels/original.json", 150, 3, "Original", nullptr} },
		{1, {"assets/levels/championship.json", 51, 4, "Championship", nullptr} },
		{2, {"assets/levels/professional.json", 150, 3, "Professional", nullptr} },
		{3, {"assets/levels/revenge.json", 17, 3, "Revenge", nullptr} },
		{4, {"assets/levels/fanbook.json", 66, 3, "Fanbook", nullptr} }
	};

	float getEnemySpeed() { return this->enemySpeed; }
	void setEnemySpeed(float enemySpeed) { this->enemySpeed = enemySpeed; }
	
	float getPlayerSpeed() { return this->playerSpeed; }
	void setPlayerSpeed(float playerSpeed) { this->playerSpeed = playerSpeed; }

	int getGameVersion() { return this->gameVersion; }
	void setGameVersion(int gameVersion) { 
		this->gameVersion = std::clamp(gameVersion, 0, GAME_VERSION_NUMBER - 1);
	}

	int* getGameVersionPointer()
	{
		return &this->gameVersion;
	}

	float* getEnemySpeedPointer()
	{
		return &this->enemySpeed;
	}

	float* getPlayerSpeedPointer()
	{
		return &this->playerSpeed;
	}

	std::array<int, 2>& getLevel() { return this->level; }
	void setLevel(int index, int value) { this->level[index] = value; }

	unsigned int getRecordingHeight() { return this->recordingHeight; }
	void setRecordingHeight(unsigned int recordingHeight) { this->recordingHeight = recordingHeight; }

	std::string getLevelFileName() { return std::get<0>(this->configurations[gameVersion]); }
	int getCurrentMainTexture() { return std::get<2>(this->configurations[gameVersion]); }
	std::shared_ptr<std::vector<std::shared_ptr<Text>>> getCurrentMainMenuTexts() { return std::get<4>(this->configurations[gameVersion]); }

	unsigned int getFramesPerSec() { return this->framesPerSec; }
	void setFramesPerSec(unsigned int framesPerSec) { this->framesPerSec = framesPerSec; }

	void validateLevel(int& newLevel) {
		auto& currentConfiguration = configurations[gameVersion];

		short maxLevelNumber = std::get<1>(currentConfiguration);
		newLevel = newLevel > maxLevelNumber ? (newLevel < maxLevelNumber + 10 ? newLevel % maxLevelNumber : maxLevelNumber ) : newLevel;
		newLevel = newLevel < 1 ? (-10 < newLevel ? maxLevelNumber + newLevel : 1) : newLevel;
	}

	void loadMainMenuTexts(nlohmann::json translation) {
		std::function<Text(nlohmann::json, std::string, std::string, float, float)> getTextValue = [](nlohmann::json translation, std::string key, std::string defaultValue, int defaultX, int defaultY) -> Text {
			nlohmann::json properties = translation.value(key, nlohmann::json::object());
			
			std::string value = properties.value("value", defaultValue);
			float x = properties.value("x", defaultX);
			float y = properties.value("y", defaultY);

			return Text(value, { x,y });
			};

		auto originalTexts = std::make_shared<std::vector<std::shared_ptr<Text>>>(
			std::initializer_list<std::shared_ptr<Text>>{
				std::make_shared<Text>(getTextValue(translation, "player1", "PLAYER1", 12, 13)),
				std::make_shared<Text>(getTextValue(translation, "player2", "2 PLAYERS", 12, 15)),
				std::make_shared<Text>(getTextValue(translation, "editMode", "EDIT MODE", 12, 17)),
	
				std::make_shared<Text>(getTextValue(translation, "copyRight", "COPYRIGHT © 1984 HUDSON SOFT", 2, 23)),
				std::make_shared<Text>(getTextValue(translation, "permission", "WITH PERMISSION OF", 7, 24)),
				std::make_shared<Text>(getTextValue(translation, "software", "BRODERBUND SOFTWARE INC", 4, 25)),
				std::make_shared<Text>(getTextValue(translation, "allRights", "ALL RIGHTS RESERVED", 7, 26))
			}
		);

		auto championshipTexts = std::make_shared<std::vector<std::shared_ptr<Text>>>(
			std::initializer_list<std::shared_ptr<Text>>{
				std::make_shared<Text>(getTextValue(translation, "cStart", "PRESS START BUTTON", 8, 20)),
				std::make_shared<Text>(getTextValue(translation, "cCopyRight", "COPYRIGHT 1984  DOUG SMITH", 3, 23)),
				std::make_shared<Text>(getTextValue(translation, "cPublish", "PUBLISHED BY HUDSON SOFT", 4, 24)),
				std::make_shared<Text>(getTextValue(translation, "cLicense", "UNDER LICENSE FROM", 7, 25)),
				std::make_shared<Text>(getTextValue(translation, "cSoftware", "BRODERBUND SOFTWARE INC", 4, 26)),
			}
		);

		std::get<4>(configurations[0]) = originalTexts;
		std::get<4>(configurations[1]) = championshipTexts;
		std::get<4>(configurations[2]) = originalTexts;
		std::get<4>(configurations[3]) = originalTexts;
		std::get<4>(configurations[4]) = originalTexts;
	}

#ifndef NDEBUG
	bool* getEnemyDebugState() { return &this->debugEnemy; }
#endif
};

#endif