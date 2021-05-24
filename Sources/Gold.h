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
	void Draw();
	short releaseCounter = 0;
	Vector2D Pos;

public:
	Gold(Vector2DInt Pos) { this->Pos = { (float)Pos.x, (float) Pos.y}; }
	void Initialize(int, Vector2D);
	static void DrawGolds();
	static bool GoldChecker(int, int);
	static std::unique_ptr<Gold> GoldCollectChecker(float, float);
	static void ClearGoldHolders();
	static void addGoldToUncollected(std::unique_ptr<Gold>);
	static void addGoldToCollected(std::unique_ptr<Gold>);

	static short GetCollectedSize();
	static short GetUncollectedSize();

	void SetReleaseCounter(short releaseCounter) { this->releaseCounter = releaseCounter; }
	bool shouldBeReleased();
	void SetPos(Vector2D Pos) { this->Pos = Pos; }
};

#endif // !GOLD_H
