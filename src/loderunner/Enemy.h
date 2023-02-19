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

#include "GameContext.h"

class Gold;
class Play;

class Enemy {
private:
	int bestPath = 0.0f;
	float bestRating = 0.0f;

	//factor to slow or fasten animationSpeed
	int animationFactor = 20;

	float holeIdle = 2.2f;
	float holeHorizontalTime = 0.25;

	//PathFinding
	void scanFloor();
	void scanDown(int, int);
	void scanUp(int, int);

	bool enemyChecker(float, float);

	virtual void checkCollisionWithOthers();

	void fallingToPit();
	void movingInPit();
	void climbing();

	Brick* holeBrick = nullptr;
	
protected:
	std::shared_ptr<Gold> carriedGold;

	float gameTime;

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
	float charSpeed = 0.0f;

	Direction direction;
	int textureRef;

	PitState pitState;

	float* positionPointer;
	int* texturePointer;
	int* directionPointer;
	int* carryGoldPointer;

	std::shared_ptr<GameContext> gameContext;

	Vector2D pos;
	Vector2D dPos;
	Vector2D prevPos;
	Vector2D dPrevPos;
public:
	virtual ~Enemy() {}
	Enemy(float, float);

	//static std::shared_ptr<Enemy> player;

	Vector2D getPos()
	{
		return this->pos;
	}

	int getTextureRef()
	{
		return this->textureRef;
	}

	virtual void setCharSpeed(float);

	void handle();

	void checkDeath(int, int);

	virtual void die();		

	void setPositionPointer(float* positionPointer)
	{
		this->positionPointer = positionPointer;
	}

	void setTexturePointer(int* texturePointer)
	{
		this->texturePointer = texturePointer;
	}

	void setDirectionPointer(int* directionPointer)
	{
		this->directionPointer = directionPointer;
	}

	void setCarryGoldPointer(int* carryGoldPointer)
	{
		this->carryGoldPointer = carryGoldPointer;
	}

	void setTextureRef(int textureRef)
	{
		this->textureRef = textureRef;
		*texturePointer = textureRef;
	}

	bool carriesGold()
	{
		return carriedGold != nullptr;
	}

	void setGameContext(std::shared_ptr<GameContext> gameContext);
};

#endif // !ENEMY_H