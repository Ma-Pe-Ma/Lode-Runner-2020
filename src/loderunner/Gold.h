#ifndef GOLD_H
#define GOLD_H

#include "Structs/Vector2DInt.h"
#include "Structs/Vector2D.h"

#include <vector>
#include <memory>

class Enemy;

class Gold {
	static std::vector<std::unique_ptr<Gold>> uncollectedGold;
	static std::vector<std::unique_ptr<Gold>> collectedGold;

	//Enemy* carrier;
	void draw();
	short releaseCounter = 0;
	Vector2D pos;

public:
	Gold(Vector2DInt pos) { this->pos = { (float) pos.x, (float) pos.y}; }
	void initialize(int, Vector2D);
	static void drawGolds();
	static bool goldChecker(int, int);
	static std::unique_ptr<Gold> goldCollectChecker(float, float);
	static void clearGoldHolders();
	static void addGoldToUncollected(std::unique_ptr<Gold>);
	static void addGoldToCollected(std::unique_ptr<Gold>);

	static short getCollectedSize();
	static short getUncollectedSize();

	void setReleaseCounter(short releaseCounter) { this->releaseCounter = releaseCounter; }
	bool shouldBeReleased();
	void setPos(Vector2D pos) { this->pos = pos; }
};

#endif // !GOLD_H
