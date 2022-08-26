#ifndef TRAPDOOR_H
#define TRAPDOOR_H

#include "Drawing.h"
#include "Structs/Vector2DInt.h"

class Trapdoor {
	int textureRef = 0;
	Vector2DInt pos;
public:
	Trapdoor(Vector2DInt pos) { this->pos = pos; }
	void setRevealed() { this->textureRef = 24;}
	void handle() {
		Drawing::drawLevel(pos.x, pos.y, textureRef);		
	}
};

#endif