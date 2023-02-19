#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

class Player;
class Enemy;
class Brick;
class Gold;
class RenderingManager;
class Trapdoor;

class Play;

#include "Enums/LayoutBlock.h"
#include "Structs/Vector2DInt.h"

#include <memory>
#include <vector>

class GameContext {
	float enemySpeed = 0.415f;
	float playerSpeed = 0.9f;

	std::vector<std::tuple<int,int>> finishingLadders;
	short highestLadder = 30;

	std::vector<std::shared_ptr<Brick>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;

	std::shared_ptr<RenderingManager> renderingManager;

	std::vector<std::shared_ptr<Enemy>> enemies;

	std::shared_ptr<Player> player;

	int killCounter = 0;

	LayoutBlock** layout;
	std::shared_ptr<Brick>** bricks = nullptr;
	std::shared_ptr<Trapdoor>**trapdoors = nullptr;

	int randomDebris;

	int* pointerToDebrisTexture;
	int* pointerToDebrisLocation;

	std::vector<std::shared_ptr<Gold>> uncollectedGoldList;
	std::vector<std::shared_ptr<Gold>> collectedGoldList;

	Play* play;
public:
	void setBrickList(std::vector<std::shared_ptr<Brick>>);
	void setTrapdoorList(std::vector<std::shared_ptr<Trapdoor>>);

	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager);

	void setEnemies(std::vector<std::shared_ptr<Enemy>> enemies);
	std::vector<std::shared_ptr<Enemy>> getEnemies();

	void setPlayer(std::shared_ptr<Player> player);
	std::shared_ptr<Player> getPlayer();

	void run();

	void checkDeaths(int, int);
	void notifyPlayerAboutDigEnd();

	void handlePlayerDying();

	void setEnemySpeed(float enemySpeed)
	{
		this->enemySpeed = enemySpeed;
	}

	float getEnemySpeed()
	{
		return this->enemySpeed;
	}

	void setPlayerSpeed(float playerSpeed)
	{
		this->playerSpeed = playerSpeed;
	}

	float getPlayerSpeed() {
		return this->playerSpeed;
	}

	bool enemyCarriesGold();

	bool GameContext::checkDigPrevention(int x, int y);

	void clearContainers();

	void setLayout(LayoutBlock** layout)
	{
		this->layout = layout;
	}

	LayoutBlock** getLayout()
	{
		return this->layout;
	}

	void generateFinishingLadders();

	void setFinishingLadders(std::vector<std::tuple<int,int>> finishingLadders)
	{
		this->finishingLadders = finishingLadders;
	}

	int getHighestLadder()
	{
		return this->highestLadder;
	}

	void setHighestLadder(int highestLadder)
	{
		this->highestLadder = highestLadder;
	}

	int getKillCounter()
	{
		return killCounter;
	}

	void incrementKillCounter()
	{
		this->killCounter++;
	}

	void setBricks(std::shared_ptr<Brick>** bricks)
	{
		this->bricks = bricks;
	}

	std::shared_ptr<Brick>** getBricks()
	{
		return this->bricks;
	}

	void setTrapdoors(std::shared_ptr<Trapdoor>** trapDoors)
	{
		this->trapdoors = trapDoors;
	}

	std::shared_ptr<Trapdoor>** getTrapdoors()
	{
		return this->trapdoors;
	}

	void setPointerToDebrisTexture(int* pointerToDebrisTexture)
	{
		this->pointerToDebrisTexture = pointerToDebrisTexture;
	}

	int* getPointerToDebrisTexture()
	{
		return this->pointerToDebrisTexture;
	}

	void setPointerToDebrisLocation(int* pointerToDebrisLocation)
	{
		this->pointerToDebrisLocation = pointerToDebrisLocation;
	}

	int* getPointerToDebrisLocation()
	{
		return this->pointerToDebrisLocation;
	}

	void setRandomDebris(int randomDebris)
	{
		this->randomDebris = randomDebris;
	}

	int getRandomDebris()
	{
		return this->randomDebris;
	}

	std::vector<std::shared_ptr<Gold>> getUncollectedGoldList()
	{
		return this->uncollectedGoldList;
	}

	void setUncollectedGoldList(std::vector<std::shared_ptr<Gold>> uncollectedGoldList)
	{
		this->uncollectedGoldList = uncollectedGoldList;
	}

	std::vector<std::shared_ptr<Gold>> getCollectedGoldList()
	{
		return this->collectedGoldList;
	}

	void setCollectedGoldList(std::vector<std::shared_ptr<Gold>> collectedGoldList)
	{
		this->collectedGoldList = collectedGoldList;
	}

	short getCollectedGoldSize() {
		return collectedGoldList.size();
	}
	short getUncollectedGoldSize() {
		return uncollectedGoldList.size();
	}

	std::shared_ptr<Gold> goldCollectChecker(float, float);
	void addGoldToCollectedList(std::shared_ptr<Gold> collectedGoldList);
	void addGoldToUncollectedList(std::shared_ptr<Gold> newGold);
	bool goldChecker(int x, int y);

	void transitionToDeath();
	void transitionToOutro();

	void setPlayState(Play* play)
	{
		this->play = play;
	}

	float* getEnemySpeedPointer()
	{
		return &enemySpeed;
	}

	float* getPlayerSpeedPointer()
	{
		return &playerSpeed;
	}
};

#endif