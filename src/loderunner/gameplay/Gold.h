#ifndef GOLD_H
#define GOLD_H

#include "Vector2DInt.h"
#include "Vector2D.h"

#include <vector>
#include <memory>

class Enemy;

class Gold {
	Vector2DInt pos;
	int* positionPointer;

public:
	Gold(Vector2DInt pos) { this->pos = { pos.x, pos.y}; }

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

	friend class GameContext;
	friend class GameElements;
};

#endif // !GOLD_H
