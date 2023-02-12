#ifndef TRAPDOOR_H
#define TRAPDOOR_H

#include "Drawing.h"
#include "Structs/Vector2DInt.h"

class Trapdoor {
	int* texturePointer;
	Vector2DInt pos;
public:
	Trapdoor(Vector2DInt pos) { this->pos = pos; }
	void setRevealed()
	{ 
		*(this->texturePointer) = 24;
	}

	void setTexturePointer(int* texturePointer)
	{
		this->texturePointer = texturePointer;
	}

	Vector2DInt getPos()
	{
		return this->pos;
	}
};

#endif