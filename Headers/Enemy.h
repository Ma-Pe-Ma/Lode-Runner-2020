#ifndef ENEMY_H
#define ENEMY_H

#include <cstdlib>

#include "Enums/Direction.h"
#include "Enums/EnemyState.h"
#include "Enums/PitState.h"

#include "Structs/Vector2D.h"
#include "Structs/Vector2DInt.h"

#include "Audio.h"
#include "Variables.h"

class Gold;

class Enemy {
private:
	void FreeRun();
	void Digging();
	void Dying();
	void Falling();
	void Pitting();
	void StartingToFall();
	void IntoPit();
	void OutFromPit();

	void ScanFloor();
	void ScanDown(int, int);
	void ScanUp(int, int);

	bool EnemyChecker(float, float);

public:
	static bool EnemyCheckerGlobal(float, float);

	int index;
	static int enemyNr;
	static Gold* gold;
	Gold* carriedGold;

	float charSpeed;

	Direction direction;
	int TextureRef;

	float inHoleTime;
	float dieTimer;

	EnemyState state;
	PitState pitState;

	Vector2D Pos;
	Vector2D dPos;
	Vector2D prevPos;
	Vector2D dPrevPos;
	Vector2DInt holePos;

	int goldVariable = 0;

	void Initialize(int i, Vector2D);
	void Move();
	void Animation();
	int PathFinding();

	static int bestPath;
	static float bestRating;
	static int GoldChecker(float, float);

	Enemy() {}

	static Enemy enemies[50];
};

#endif // !ENEMY_H