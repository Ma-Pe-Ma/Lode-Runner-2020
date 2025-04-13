#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include <string>
#include <algorithm>

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
	//game version - {filename, maxlevel, textureID, name}
	std::map<int, std::tuple<std::string, short, int, std::string>> configurations = {
		{0, {"assets/levels/original.json", 150, 3, "Original"}},
		{1, {"assets/levels/championship.json", 51, 4, "Championship"}},
		{2, {"assets/levels/professional.json", 150, 3, "Professional"}},
		{3, {"assets/levels/revenge.json", 17, 3, "Revenge"}},
		{4, {"assets/levels/fanbook.json", 66, 3, "Fanbook"}}
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

	unsigned int getFramesPerSec() { return this->framesPerSec; }
	void setFramesPerSec(unsigned int framesPerSec) { this->framesPerSec = framesPerSec; }

	void validateLevel(int& newLevel) {
		auto& currentConfiguration = configurations[gameVersion];

		short maxLevelNumber = std::get<1>(currentConfiguration);
		newLevel = newLevel > maxLevelNumber ? newLevel % maxLevelNumber : newLevel;
		newLevel = newLevel < 1 ? maxLevelNumber + newLevel : newLevel;
	}

#ifndef NDEBUG
	bool* getEnemyDebugState() { return &this->debugEnemy; }
#endif
};

#endif