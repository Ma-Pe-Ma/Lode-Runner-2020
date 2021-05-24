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
	void ScanFloor();
	void ScanDown(int, int);
	void ScanUp(int, int);

	bool EnemyChecker(float, float);
	void handle();

	virtual void CheckCollisionWithOthers();

	void FallingToPit();
	void MovingInPit();
	void Climbing();

	Brick* holeBrick = nullptr;
	
protected:
	static unsigned int killCounter;
	static float playerSpeed;
	static float enemySpeed;
	
	static LayoutBlock** layout;
	static std::unique_ptr<Brick>** brickO;
	static std::unique_ptr<Trapdoor>** trapdoors;
	static Play* play;
	static float gameTime;
	
	static std::unique_ptr<Enemy> player;

	inline void DetermineDirection();
	void DetermineNearbyObjects();
	LayoutBlock middle;
	LayoutBlock downBlock;
	int curX;
	int curY;
	short directionX;
	short directionY;

	virtual void InitiateFallingStart();
	virtual void InitiateFallingStop();
		
	void CheckCollisionsWithEnvironment();

	void LadderTransformation();

	TextureMap textureMap = { 8,16,12,0,4,-1};

	virtual void FindPath();
	void Move();
	virtual void Dying();

	//Moving 
	virtual void FreeRun();
	virtual void Digging();
	virtual void Falling();
	void Pitting();
	void StartingToFall();

	//Animating
	void Animate();
	virtual void AnimateFreeRun();
	virtual void AnimateGoing();
	virtual void AnimateOnLadder();
	virtual void AnimateOnPole();

	virtual void AnimateDigging();
	virtual void AnimateDying();
	virtual void AnimateFalling();
	virtual void AnimatePitting();
	
	virtual void CheckGoldCollect();
	virtual void CheckGoldDrop();
	
	virtual void ReleaseFromDigging() {};

	virtual bool CheckHole();

	EnemyState state;
	float holeTimer;
	float dieTimer;
	float actualSpeed;
	float charSpeed;

	Direction direction;
	int TextureRef;

	PitState pitState;
public:
	virtual ~Enemy() {}
	static std::vector<std::unique_ptr<Enemy>> enemies;
	static void clearEnemyVector();
	static void AddEnemy(Vector2DInt);

	static bool EnemyCheckerGlobal(float, float);
	static void handleCharacters();

	static bool CheckDigPrevention(int, int);
	static void CheckDeaths(int, int);
	void CheckDeath(int, int);

	virtual void Die();

	Enemy();

	std::unique_ptr<Gold> carriedGold;

	Vector2D Pos;
	Vector2D dPos;
	Vector2D prevPos;
	Vector2D dPrevPos;

	static void setLayoutPointers(LayoutBlock** layout, std::unique_ptr<Brick>**, std::unique_ptr<Trapdoor>**, Play*);
	static void NotifyPlayerAboutDigEnd();
	static bool HasGold();

	static void SetPlayerSpeed(float);
	static void SetEnemySpeed(float);
	static unsigned int GetKillCounter();
	static void DrawPaused();
	static void DrawPlayerDeath();
	static void HandlePlayerDying();
};

#endif // !ENEMY_H