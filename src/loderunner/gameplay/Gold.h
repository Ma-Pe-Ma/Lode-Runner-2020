#ifndef GOLD_H
#define GOLD_H

#include "Vector2DInt.h"
#include "Vector2D.h"

#include <vector>
#include <memory>

class Enemy;

class Gold {
	short releaseCounter = 0;
	Vector2DInt pos;
	int* positionPointer;

public:
	Gold(Vector2DInt pos) { this->pos = { pos.x, pos.y}; }

	void setReleaseCounter(short releaseCounter) { this->releaseCounter = releaseCounter; }
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

	bool shouldBeReleased() {
		if (releaseCounter-- <= 0) {
			return true;
		}

		return false;
	}
};

#endif // !GOLD_H
