#ifndef GAMEELEMENTS_H
#define GAMEELEMENTS_H

#include <optional>
#include <vector>
#include <array>
#include <memory>
#include <chrono>

#include "LayoutBlock.h"
#include "Vector2DInt.h"

class Trapdoor;
class Fruit;
class Gold;
class Enemy;
class Brick;

using Layout = std::array<std::array<LayoutBlock, 18>, 30>;

struct Fruit {
	std::optional<int> id;
	float* position;
	std::tuple<float, float> time;
	std::optional<bool> collected;
};

class GameElements {
	short randomDebris = 0;
	int going = 0;

	int* pointerToDebrisTexture;
	int* pointerToDebrisLocation;

	int highestLadder = 0;
	int killCounter = 0;

	std::vector<std::shared_ptr<Brick>> brickList;
	std::shared_ptr<Brick> diggedBrick;

	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;

	std::vector<std::tuple<int, int>> finishingLadders;
	std::vector<std::shared_ptr<Gold>> collectedGoldList;
	std::vector<std::shared_ptr<Gold>> enemyGoldList;
	std::vector<std::shared_ptr<Gold>> uncollectedGoldList;

	std::shared_ptr<Fruit> fruit;
	std::shared_ptr<Enemy> player;
	std::vector<std::shared_ptr<Enemy>> enemies;

public:
	Layout layout;

	bool isBrick(int x, int y);
	bool isEnemy (float x, float y, float thresholdX = 0.5f, float thresholdY = 0.5f);
	bool isGold (int x, int y);

	bool isEnemyUnder(Enemy*);
	bool isEnemyUnderNext(Enemy*, float);
	bool isEnemyUnderFalling(Enemy* checkableEnemy, float nextPos);
	bool isEnemyNearHole(Enemy* checkableEnemy, float x, float y);

	Vector2DInt getPlayerPosition();
	EnemyState getPlayerState();

	friend class GameContext;
	friend class LevelLoader;
};

#endif