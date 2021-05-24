#ifndef TRAPDOOR_H
#define TRAPDOOR_H

#include "Drawing.h"
#include "Structs/Vector2DInt.h"

class Trapdoor {
	int textureRef = 0;
	Vector2DInt Pos;
public:
	Trapdoor(Vector2DInt Pos) { this->Pos = Pos; }
	void setRevealed() { this->textureRef = 24;}
	void handle() {
		DrawLevel(Pos.x, Pos.y, textureRef);		
	}
};

#endif