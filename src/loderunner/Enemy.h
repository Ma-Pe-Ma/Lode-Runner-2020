#ifndef ENEMY_H
#define ENEMY_H

#include <cstdlib>

#include <iostream>

#include "Enums/Direction.h"
#include "Enums/EnemyState.h"
#include "Enums/PitState.h"

#include "Structs/Vector2D.h"
#include "Structs/Vector2DInt.h"

#include "Audio.h"

#include "Enums/LayoutBlock.h"

#include "TextureMap.h"

#include "Trapdoor.h"
#include "Brick.h"

class Gold;
class Play;

class Enemy {
private:
	static int bestPath;
	static float bestRating;

	static void moveCharacters();
	//factor to slow or fasten animationSpeed
	static int animationFactor;

	float holeIdle = 2.2f;
	float holeHorizontalTime = 0.25;

	//PathFinding
	void scanFloor();
	void scanDown(int, int);
	void scanUp(int, int);

	bool enemyChecker(float, float);
	void handle();

	virtual void checkCollisionWithOthers();

	void fallingToPit();
	void movingInPit();
	void climbing();

	Brick* holeBrick = nullptr;
	
protected:
	static unsigned int killCounter;	
	
	static LayoutBlock** layout;
	static std::unique_ptr<Brick>** brickO;
	static std::unique_ptr<Trapdoor>** trapdoors;
	static Play* play;
	static float gameTime;

	inline void determineDirection();
	void determineNearbyObjects();
	LayoutBlock middle;
	LayoutBlock downBlock;
	int curX;
	int curY;
	short directionX;
	short directionY;

	virtual void initiateFallingStart();
	virtual void initiateFallingStop();
		
	void checkCollisionsWithEnvironment();

	void ladderTransformation();

	TextureMap textureMap = { 8,16,12,0,4,-1};

	virtual void findPath();
	void move();
	virtual void dying();

	//Moving 
	virtual void freeRun();
	virtual void digging();
	virtual void falling();
	void pitting();
	void startingToFall();

	//Animating
	void animate();
	virtual void animateFreeRun();
	virtual void animateGoing();
	virtual void animateOnLadder();
	virtual void animateOnPole();

	virtual void animateDigging();
	virtual void animateDying();
	virtual void animateFalling();
	virtual void animatePitting();
	
	virtual void checkGoldCollect();
	virtual void checkGoldDrop();
	
	virtual void releaseFromDigging() {};

	virtual bool checkHole();

	EnemyState state;
	float holeTimer;
	float dieTimer;
	float actualSpeed;
	float charSpeed;

	Direction direction;
	int textureRef;

	PitState pitState;
public:
	virtual ~Enemy() {}

	static std::unique_ptr<Enemy> player;
	static std::vector<std::unique_ptr<Enemy>> enemies;
	static void clearEnemyVector();
	static void addEnemy(Vector2DInt);

	static bool enemyCheckerGlobal(float, float);
	static void handleCharacters();

	static bool checkDigPrevention(int, int);
	static void checkDeaths(int, int);
	void checkDeath(int, int);

	virtual void die();

	Enemy(float, float);

	std::unique_ptr<Gold> carriedGold;

	Vector2D pos;
	Vector2D dPos;
	Vector2D prevPos;
	Vector2D dPrevPos;
	
	virtual void updateCharSpeed();

	static void setLayoutPointers(LayoutBlock** layout, std::unique_ptr<Brick>**, std::unique_ptr<Trapdoor>**, Play*);
	static void notifyPlayerAboutDigEnd();
	static bool hasGold();

	static float enemySpeed;
	static float playerSpeed;

	static void setPlayerSpeed(float);
	static void setEnemySpeed(float);
	static unsigned int getKillCounter();
	static void drawPaused();
	static void drawPlayerDeath();
	static void handlePlayerDying();
};

#endif // !ENEMY_H