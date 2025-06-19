#ifndef ENEMY_H
#define ENEMY_H

#include <cstdlib>

#include <iostream>
#include <optional>

#include "Vector2D.h"
#include "Vector2DInt.h"

#include "LayoutBlock.h"
#include "GameElements.h"

#include "Trapdoor.h"

class GameElements;
class GameContext;
class Play;

enum class Direction {
	right,
	left
};

struct EnemyTextureMap {
	short death = 8;
	short falling = 16;
	short going = 12;
	short ladder = 0;
	short pole = 4;
	short idle = -1;
};

class Enemy {
private:
	// time parameters
	const float holeIdle = 2.2f;
	const float holeHorizontalTime = 0.25f;

	//factors used for determining the next step
	int bestPath = 0.0f;
	float bestRating = 0.0f;

	//PathFinding
	Vector2DInt scanFloor(std::shared_ptr<GameElements>&);
	void scanDown(int, int, std::shared_ptr<GameElements>&);
	void scanUp(int, int, std::shared_ptr<GameElements>&);

	std::optional<int> goldCounter;	
	std::optional<Vector2D> hole;
protected:
	virtual Vector2DInt findPath(std::shared_ptr<GameElements>&);
	
	Vector2D ladderTransformation(Vector2D, std::shared_ptr<GameElements>&);
	Vector2D checkCollisionsWithEnvironment(Vector2D, std::shared_ptr<GameElements>&);
	virtual Vector2D handleDropping(Vector2D, std::shared_ptr<GameElements>&);

	virtual Vector2D falling(float, std::shared_ptr<GameElements>&);
	Vector2D pitting(Vector2D, float, float, std::shared_ptr<GameElements>&);

	LayoutBlock middle;
	LayoutBlock downBlock;
	Vector2DInt current = { 0, 0 };

	EnemyTextureMap enemyTextureMap;

	EnemyState state = EnemyState::freeRun;
	float timer = 0.0f;

	Direction direction = Direction::right;

	float* positionPointer;
	int* texturePointer;
	int* directionPointer;
	int* carryGoldPointer;

	Vector2D pos;
	Vector2D dPos;
public:
	Enemy(float, float, bool player = false);

	EnemyTextureMap getTextureMap() { return this->enemyTextureMap; }

	void setPositionPointer(float* positionPointer)	{ this->positionPointer = positionPointer; }
	void setTexturePointer(int* texturePointer) { this->texturePointer = texturePointer; }
	void setDirectionPointer(int* directionPointer) { this->directionPointer = directionPointer; }
	void setCarryGoldPointer(int* carryGoldPointer)	{ this->carryGoldPointer = carryGoldPointer; }
	void setTexture(int texture) { *texturePointer = texture; }

	void setPosition(Vector2D pos) {
		this->pos = pos;
		positionPointer[0] = pos.x;
		positionPointer[1] = pos.y;
	}

	void determineNearbyObjects(std::shared_ptr<GameElements>&, Vector2D = {0.0f, 0.0f});

	friend class GameContext;
	friend class GameElements;
	friend class Outro;
	friend class RenderingManager;
};

#endif // !ENEMY_H