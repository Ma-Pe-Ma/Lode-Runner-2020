#ifndef GOLD_H
#define GOLD_H

#include "Structs/Vector2DInt.h"
#include "Structs/Vector2D.h"

class Enemy;

class Gold {

	bool collected;
	Enemy* carrier;
	int index;

public:
	Vector2D Pos;
	void Initialize(int, Vector2D);
	static int goldNr;
	static int remainingGoldCount;

	static Gold gold[100];
};

#endif // !GOLD_H
