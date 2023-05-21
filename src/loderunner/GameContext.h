#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

class Player;
class Enemy;
class Brick;
class Gold;
class Trapdoor;

class Play;

class Audio;
class RenderingManager;
class IOContext;

#include "Enums/LayoutBlock.h"
#include "Structs/Vector2DInt.h"

#include <memory>
#include <vector>

#include "iocontext/GameConfiguration.h"

#include <iostream>

class GameContext {
	std::shared_ptr<RenderingManager> renderingManager;
	std::shared_ptr<GameConfiguration> gameConfiguration;
	std::shared_ptr<Audio> audio;
	std::shared_ptr<IOContext> ioContext;

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

	int* pointerToDebrisTexture;
	int* pointerToDebrisLocation;

	Play* play;

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