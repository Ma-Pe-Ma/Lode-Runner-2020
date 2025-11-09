#ifndef GOLD_H
#define GOLD_H

#include "Vector2DInt.h"
#include "Vector2D.h"

#include <vector>
#include <memory>

class Enemy;

class Gold {
	Vector2D pos;
	float* positionPointer;

public:
	Gold(Vector2D pos) { this->pos = { pos.x, pos.y}; }

	Vector2D getPos() { return this->pos; }

	void setPos(Vector2D pos) { 
		this->pos = pos;
		positionPointer[0] = pos.x;
		positionPointer[1] = pos.y;
	}

	void setPositionPointer(float* positionPointer)
	{
		this->positionPointer = positionPointer;
	}

	friend class GameContext;
	friend class GameElements;
};

#endif // !GOLD_H
