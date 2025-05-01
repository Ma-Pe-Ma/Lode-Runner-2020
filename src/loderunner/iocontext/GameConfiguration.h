#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include <string>
#include <algorithm>

#include <json.hpp>

#include "rendering/Text.h"

#define GAME_VERSION_NUMBER 5

#include "Translation.h"

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

	std::shared_ptr<Translation> translation;
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
#ifndef NDEBUG
	bool* getEnemyDebugState() { return &this->debugEnemy; }
#endif

	void loadTranslations(std::string defaultLanguage, nlohmann::json translation) {
		this->translation = std::make_shared<Translation>(translation);
		this->translation->loadGenericTexts();

		auto languages = this->translation->getLanguages();
		auto it = std::find(languages.begin(), languages.end(), defaultLanguage);
		int currentLanguageIndex = it != languages.end() ? std::distance(languages.begin(), it) : 0;
		this->translation->setCurrentLanguageIndex(currentLanguageIndex);

		updateMainMenuTexts();
	}

	void updateMainMenuTexts() {
		std::shared_ptr<std::vector<std::shared_ptr<Text>>> mainMenuText = std::make_shared<std::vector<std::shared_ptr<Text>>>();

		for (auto s : { "player1", "player2", "editMode", "copyRight", "permission", "software", "allRights" }) {
			mainMenuText->push_back(std::make_shared<Text>(this->translation->getTranslationText(s)));
		}

		for (auto i : { 0,2,3,4 }) {
			std::get<4>(configurations[i]) = mainMenuText;
		}

		std::shared_ptr<std::vector<std::shared_ptr<Text>>> championText = std::make_shared<std::vector<std::shared_ptr<Text>>>();

		for (auto s : { "cStart", "cCopyRight", "cPublish", "cLicense", "cSoftware" }) {
			championText->push_back(std::make_shared<Text>(this->translation->getTranslationText(s)));
		}
		std::get<4>(configurations[1]) = championText;
	}

	std::string changeLanguage(int index) {
		this->translation->setCurrentLanguageIndex(index);
		updateMainMenuTexts();

		return this->translation->getLanguages()[index];
	}
	
	std::vector<std::string> getLanguages() {
		return this->translation->getLanguages();
	}

	std::shared_ptr<Translation> getTranslation() {
		return this->translation;
	}
};

#endif