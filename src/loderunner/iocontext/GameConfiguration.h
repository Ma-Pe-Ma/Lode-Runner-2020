#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include <string>

class GameConfiguration {
	float enemySpeed = 0.415f;
	float playerSpeed = 0.9f;

	int gameVersion = 0;
	unsigned int startingLevel = 1;
	unsigned int recordingHeight = 800;

	std::string levelFileName = "Assets/Level/ChampionshipLevels.txt";

	unsigned int framesPerSec;

#ifndef NDEBUG
	bool debugEnemy = false;
#endif

public:
	float getEnemySpeed() { return this->enemySpeed; }
	void setEnemySpeed(float enemySpeed) { this->enemySpeed = enemySpeed; }
	
	float getPlayerSpeed() { return this->playerSpeed; }
	void setPlayerSpeed(float playerSpeed) { this->playerSpeed = playerSpeed; }

	int getGameVersion() { return this->gameVersion; }
	void setGameVersion(int gameVersion) { 
		gameVersion = gameVersion > 1 ? 1 : gameVersion;
		gameVersion = gameVersion < 0 ? 0 : gameVersion;
		this->gameVersion = gameVersion;

		switch (this->gameVersion) {
		case 0:
			levelFileName = "Assets/Level/OriginalLevels.txt";
			break;
		case 1:
			levelFileName = "Assets/Level/ChampionshipLevels.txt";
			break;
		default:

			break;
		}
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

	unsigned int getStartingLevel() { return this->startingLevel; }
	void setStartingLevel(unsigned int startingLevel) { this->startingLevel = startingLevel; }

	unsigned int getRecordingHeight() { return this->recordingHeight; }
	void setRecordingHeight(unsigned int recordingHeight) { this->recordingHeight = recordingHeight; }

	std::string getLevelFileName() { return this->levelFileName; }
	void setLevelFileName(std::string levelFileName) { this->levelFileName = levelFileName; }

	unsigned int getFramesPerSec() { return this->framesPerSec; }
	void setFramesPerSec(unsigned int framesPerSec) { this->framesPerSec = framesPerSec; }

	void validateLevel(int& newLevel) {
		int maxLevelNumber = gameVersion == 0 ? 150 : 51;
		newLevel = newLevel < 1 ? maxLevelNumber + newLevel : newLevel;
		newLevel = newLevel > maxLevelNumber ? newLevel - maxLevelNumber : newLevel;
	}

#ifndef NDEBUG
	bool* getEnemyDebugState() { return &this->debugEnemy; }
#endif
};

#endif