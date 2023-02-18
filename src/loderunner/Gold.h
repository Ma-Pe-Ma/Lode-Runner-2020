#ifndef GOLD_H
#define GOLD_H

#include "Structs/Vector2DInt.h"
#include "Structs/Vector2D.h"

#include <vector>
#include <memory>

class Enemy;

class Gold {
	static std::vector<std::shared_ptr<Gold>> uncollectedGold;
	static std::vector<std::shared_ptr<Gold>> collectedGold;

	//Enemy* carrier;
	short releaseCounter = 0;
	Vector2DInt pos;
	int* positionPointer;

public:
	Gold(Vector2DInt pos) { this->pos = { pos.x, pos.y}; }
	static bool goldChecker(int, int);
	static std::shared_ptr<Gold> goldCollectChecker(float, float);
	static void clearGoldHolders();
	static void addGoldToUncollected(std::shared_ptr<Gold>);
	static void addGoldToCollected(std::shared_ptr<Gold>);

	static short getCollectedSize();
	static short getUncollectedSize();

	void setReleaseCounter(short releaseCounter) { this->releaseCounter = releaseCounter; }
	bool shouldBeReleased();
	Vector2DInt getPos() { return this->pos; }

	void setPos(Vector2DInt pos) { 
		this->pos = pos;
		positionPointer[0] = pos.x;
		positionPointer[1] = pos.y;
	}

	void setPositionPointer(int* positionPointer)
	{
		this->positionPointer = positionPointer;
	}
};

#endif // !GOLD_H
