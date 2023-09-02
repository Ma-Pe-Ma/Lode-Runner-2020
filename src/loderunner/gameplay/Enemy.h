#ifndef ENEMY_H
#define ENEMY_H

#include <cstdlib>

#include <iostream>

#include "Direction.h"
#include "EnemyState.h"
#include "PitState.h"

#include "Vector2D.h"
#include "Vector2DInt.h"

#include "LayoutBlock.h"
#include "TextureMap.h"

#include "Trapdoor.h"
#include "Brick.h"

#include "GameContext.h"

class Gold;
class Play;

class Enemy {
private:
	// time parameters
	const float holeIdle = 2.2f;
	const float holeHorizontalTime = 0.25f;

	//factors used for determining the next step
	int bestPath = 0.0f;
	float bestRating = 0.0f;

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
	//factor to slow or fasten animationSpeed
	int animationFactor = 4;
	int animationTimeFactor;

	GameContext* gameContext;

	void determineNearbyObjects();
	void ladderTransformation();
	void checkCollisionsWithEnvironment();

	virtual void checkGoldCollect();
	virtual void checkGoldDrop();

	virtual void releaseFromDigging() {};
	virtual bool checkHole();

	virtual void initiateFallingStart();
	virtual void initiateFallingStop();	

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
	
	float gameTime;
	float frameDelta;

	LayoutBlock middle;
	LayoutBlock downBlock;

	Vector2DInt current = { 0, 0 };
	Vector2DInt directionHelper = { 0, 0 };

	TextureMap textureMap = { 8, 16, 12, 0, 4, -1 };

	std::shared_ptr<Gold> carriedGold;

	EnemyState state = EnemyState::freeRun;
	float holeTimer = 0.0f;
	float dieTimer = 0.0f;
	float actualSpeed = 0.0f;
	float charSpeed = 0.0f;

	Direction direction;
	PitState pitState;	

	float* positionPointer;
	int* texturePointer;
	int* directionPointer;
	int* carryGoldPointer;

	Vector2D pos;
	Vector2D dPos;
	Vector2D prevPos;
	Vector2D dPrevPos;

#ifndef NDEBUG
	int debugEnemy = 0;
#endif
public:
	virtual ~Enemy() {}
	Enemy(float, float);

	void handle(float, float);

	bool carriesGold() { return carriedGold != nullptr; }
	void checkDeath(int, int);

	virtual void die();

	//Getters and setters
	void setGameContext(GameContext* gameContext) { this->gameContext = gameContext; }

	TextureMap getTextureMap() { return this->textureMap; }
	virtual void setCharSpeed(float charSpeed) { this->charSpeed = charSpeed; }

	void setPositionPointer(float* positionPointer)	{ this->positionPointer = positionPointer; }
	void setTexturePointer(int* texturePointer) { this->texturePointer = texturePointer; }
	void setDirectionPointer(int* directionPointer) { this->directionPointer = directionPointer; }
	void setCarryGoldPointer(int* carryGoldPointer)	{ this->carryGoldPointer = carryGoldPointer; }
	
	void setTexture(int texture) { *texturePointer = texture; }

	Vector2D getPos() {	return this->pos; }
	void setDPos(Vector2D dPos) { this->dPos = dPos; }	

	void setGameTime(float gameTime) { this->gameTime = gameTime; }

#ifndef NDEBUG
	void setDebugEnemyState(int debugEnemy) { this->debugEnemy = debugEnemy; }
#endif
};

#endif // !ENEMY_H