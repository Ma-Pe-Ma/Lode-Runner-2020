#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

class Player;
class Enemy;
class Brick;
class Gold;
class Trapdoor;
class Text;

class Play;

class Audio;
class RenderingManager;
class IOContext;

#include <map>
#include <memory>
#include <vector>
#include <chrono>

#include "iocontext/GameConfiguration.h"
#include "LayoutBlock.h"
#include "Vector2DInt.h"

class GameContext {
	std::shared_ptr<RenderingManager> renderingManager;
	std::shared_ptr<GameConfiguration> gameConfiguration;
	std::shared_ptr<Audio> audio;
	std::shared_ptr<IOContext> ioContext;

	std::vector<std::shared_ptr<Brick>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;
	LayoutBlock layout[30][18];
	//Two dimensional garbage collected arrays
	std::shared_ptr<std::shared_ptr<std::shared_ptr<Brick>[]>[]> bricks = nullptr;
	std::shared_ptr<std::shared_ptr<std::shared_ptr<Trapdoor>[]>[]> trapdoors = nullptr;

	std::vector<std::tuple<int, int>> finishingLadders;
	std::vector<std::shared_ptr<Gold>> uncollectedGoldList;
	std::vector<std::shared_ptr<Gold>> collectedGoldList;

	std::vector<std::shared_ptr<Enemy>> enemies;
	std::shared_ptr<Player> player;	

	std::shared_ptr<Text> timeText;

	short highestLadder = 30;
	int killCounter = 0;
	int randomDebris = 0;

	int* pointerToDebrisTexture;
	int* pointerToDebrisLocation;

	Play* play;

	const std::map<short, LayoutBlock> generatorLayoutMap = {
		{0, LayoutBlock::empty},
		{1, LayoutBlock::brick},
		{2, LayoutBlock::concrete},
		{3, LayoutBlock::ladder},
		{4, LayoutBlock::pole},
		{5, LayoutBlock::trapDoor},
		{6, LayoutBlock::empty},
		{7, LayoutBlock::empty},
		{8, LayoutBlock::empty},
		{9, LayoutBlock::empty},
	};

	float previousSessionSum = 0;
	std::chrono::system_clock::time_point sessionStartTime;
	std::chrono::system_clock::time_point previousFrame;
public:
	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager) { this->renderingManager = renderingManager; }
	std::shared_ptr<RenderingManager> getRenderingManager() { return this->renderingManager; }

	void setGameConfiguration(std::shared_ptr<GameConfiguration> gameConfiguration) { this->gameConfiguration = gameConfiguration; }
	std::shared_ptr<GameConfiguration> getGameConfiguration() { return this->gameConfiguration; }

	void setAudio(std::shared_ptr<Audio> audio) { this->audio = audio; }
	std::shared_ptr<Audio> getAudio() { return this->audio; }

	void setIOContext(std::shared_ptr<IOContext> ioContext) { this->ioContext = ioContext; }
	std::shared_ptr<IOContext> getIOContext() { return this->ioContext; }

	void setBrickList(std::vector<std::shared_ptr<Brick>> brickList) { this->brickList = brickList;	};
	void setTrapdoorList(std::vector<std::shared_ptr<Trapdoor>> trapdoorList) { this->trapdoorList = trapdoorList; }
	
	LayoutBlock getLayoutElement(int x, int y) { return this->layout[x][y]; }
	void setLayoutElement(int x, int y, LayoutBlock newElement) { this->layout[x][y] = newElement; }

	std::shared_ptr<Brick> getBrickByCoordinates(int x, int y) { return this->bricks[x][y]; }

	std::shared_ptr<Trapdoor> getTrapdoorByCoordinate(int x, int y) { return this->trapdoors[x][y]; }

	std::vector<std::shared_ptr<Enemy>> getEnemies() { return this->enemies; }

	std::shared_ptr<Player> getPlayer() { return this->player; }

	std::vector<std::shared_ptr<Gold>> getUncollectedGoldList() { return this->uncollectedGoldList; }
	void setUncollectedGoldList(std::vector<std::shared_ptr<Gold>> uncollectedGoldList) { this->uncollectedGoldList = uncollectedGoldList; }

	std::vector<std::shared_ptr<Gold>> getCollectedGoldList() { return this->collectedGoldList; }
	void setCollectedGoldList(std::vector<std::shared_ptr<Gold>> collectedGoldList) { this->collectedGoldList = collectedGoldList; }

	short getCollectedGoldSize() { return collectedGoldList.size(); }
	short getUncollectedGoldSize() { return uncollectedGoldList.size(); }

	void setFinishingLadders(std::vector<std::tuple<int, int>> finishingLadders) { this->finishingLadders = finishingLadders; }
	int getHighestLadder() { return this->highestLadder; }

	int getKillCounter() { return killCounter; }
	void incrementKillCounter() { this->killCounter++; }

	void setPointerToDebrisTexture(int* pointerToDebrisTexture) { this->pointerToDebrisTexture = pointerToDebrisTexture; }
	int* getPointerToDebrisTexture() { return this->pointerToDebrisTexture; }

	void setPointerToDebrisLocation(int* pointerToDebrisLocation) { this->pointerToDebrisLocation = pointerToDebrisLocation; }
	int* getPointerToDebrisLocation() { return this->pointerToDebrisLocation; }

	void setRandomDebris(int randomDebris) { this->randomDebris = randomDebris; }
	int getRandomDebris() { return this->randomDebris; }

	void setPlayState(Play* play) { this->play = play; }

	void run();

	bool checkDigPrevention(int x, int y);
	void notifyPlayerAboutDigEnd();

	void checkDeaths(int, int);
	void checkPlayerDeathByEnemy();
	void handlePlayerDying();

	bool enemyCarriesGold();
	std::shared_ptr<Gold> goldCollectChecker(float, float);
	bool goldChecker(int x, int y);

	void addGoldToCollectedList(std::shared_ptr<Gold> collectedGoldList);
	void addGoldToUncollectedList(std::shared_ptr<Gold> newGold);

	void generateFinishingLadders();

	void transitionToDeath();
	void transitionToOutro();

	void clearContainers();

	void loadLevel(unsigned int);
	void generateLevel(short[30][18]);
	
	void resetSessionLength();
	void setSessionStartTime();
	void setSessionEndTime();
	float calculateEllapsedTime();
	float calculateFrameDelta();
};

#endif