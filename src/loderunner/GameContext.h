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

#include <iostream>

class GameContext {
	std::shared_ptr<RenderingManager> renderingManager;

	std::vector<std::shared_ptr<Brick>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;
	LayoutBlock** layout;
	std::shared_ptr<Brick>** bricks = nullptr;
	std::shared_ptr<Trapdoor>** trapdoors = nullptr;

	std::vector<std::tuple<int, int>> finishingLadders;
	short highestLadder = 30;

	std::vector<std::shared_ptr<Enemy>> enemies;
	std::shared_ptr<Player> player;

	std::vector<std::shared_ptr<Gold>> uncollectedGoldList;
	std::vector<std::shared_ptr<Gold>> collectedGoldList;

	int killCounter = 0;	

	int randomDebris = 0;

	float enemySpeed = 0.415f;
	float playerSpeed = 0.9f;

	int* pointerToDebrisTexture;
	int* pointerToDebrisLocation;

	Play* play;
public:
	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager) { this->renderingManager = renderingManager; }

	void setBrickList(std::vector<std::shared_ptr<Brick>> brickList) { this->brickList = brickList;	};
	void setTrapdoorList(std::vector<std::shared_ptr<Trapdoor>> trapdoorList) { this->trapdoorList = trapdoorList; }
	
	void setLayout(LayoutBlock** layout) {	this->layout = layout; }
	LayoutBlock** getLayout() { return this->layout; }

	void setBricks(std::shared_ptr<Brick>** bricks) { this->bricks = bricks; }
	std::shared_ptr<Brick>** getBricks() { return this->bricks; }

	void setTrapdoors(std::shared_ptr<Trapdoor>** trapDoors) { this->trapdoors = trapDoors; }
	std::shared_ptr<Trapdoor>** getTrapdoors() { return this->trapdoors; }

	void setEnemies(std::vector<std::shared_ptr<Enemy>> enemies) { this->enemies = enemies; }
	std::vector<std::shared_ptr<Enemy>> getEnemies() { return this->enemies; }

	void setPlayer(std::shared_ptr<Player> player) { this->player = player; }
	std::shared_ptr<Player> getPlayer() { return this->player; }

	std::vector<std::shared_ptr<Gold>> getUncollectedGoldList() { return this->uncollectedGoldList; }
	void setUncollectedGoldList(std::vector<std::shared_ptr<Gold>> uncollectedGoldList) { this->uncollectedGoldList = uncollectedGoldList; }

	std::vector<std::shared_ptr<Gold>> getCollectedGoldList() { return this->collectedGoldList; }
	void setCollectedGoldList(std::vector<std::shared_ptr<Gold>> collectedGoldList) { this->collectedGoldList = collectedGoldList; }

	short getCollectedGoldSize() { return collectedGoldList.size(); }
	short getUncollectedGoldSize() { return uncollectedGoldList.size(); }

	void setFinishingLadders(std::vector<std::tuple<int, int>> finishingLadders) { this->finishingLadders = finishingLadders; }
	void setHighestLadder(int highestLadder) { this->highestLadder = highestLadder; }
	int getHighestLadder() { return this->highestLadder; }

	void setEnemySpeed(float enemySpeed) { this->enemySpeed = enemySpeed; }
	float getEnemySpeed() { return this->enemySpeed; }

	void setPlayerSpeed(float playerSpeed) { this->playerSpeed = playerSpeed; }
	float getPlayerSpeed() { return this->playerSpeed; }

	int getKillCounter() { return killCounter; }
	void incrementKillCounter() { this->killCounter++; }

	void setPointerToDebrisTexture(int* pointerToDebrisTexture) { this->pointerToDebrisTexture = pointerToDebrisTexture; }
	int* getPointerToDebrisTexture() { return this->pointerToDebrisTexture; }

	void setPointerToDebrisLocation(int* pointerToDebrisLocation) { this->pointerToDebrisLocation = pointerToDebrisLocation; }
	int* getPointerToDebrisLocation() { return this->pointerToDebrisLocation; }

	void setRandomDebris(int randomDebris) { this->randomDebris = randomDebris; }
	int getRandomDebris() { return this->randomDebris; }

	float* getEnemySpeedPointer() { return &enemySpeed; }
	float* getPlayerSpeedPointer() { return &playerSpeed; }

	void setPlayState(Play* play) { this->play = play; }

	void run();

	bool checkDigPrevention(int x, int y);
	void notifyPlayerAboutDigEnd();

	void checkDeaths(int, int);
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
};

#endif