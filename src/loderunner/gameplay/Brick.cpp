#include "Brick.h"

Brick::Brick(Vector2DInt position, float gameTime) {
	this->position = position;	
	timer = gameTime;
}

void Brick::handle(float gameTime) {
	switch (brickState)	{
	case BrickState::digging:
		digging(gameTime);
		break;
	case BrickState::watiting:	
		waiting(gameTime);
		break;
	case BrickState::building:
		building(gameTime);
		break;
	default:
		break;
	}
}

void Brick::digging(float gameTime) {
	//totally digged
	if (gameTime - timer >= destroyTime) {
		brickState = BrickState::watiting;
		textureState = 15;
		timer = gameTime;
	}
	//growing hole
	else {		
		textureState = 1 + int(5 * (gameTime - timer) / destroyTime) % 5;
	}		
}

void Brick::waiting(float gameTime) {
	//waiting time
	if (gameTime - timer > diggingTime - destroyTime - buildTime) {
		brickState = BrickState::building;
		timer = gameTime;
	}
}

void Brick::building(float gameTime) {
	//totally rebuilt
	if (gameTime - timer >= buildTime) {
		brickState = BrickState::rebuilt;
		textureState = 0;
	}
	//rebuilding
	else {
		textureState = 11 - int(5 * (gameTime - timer) / buildTime) % 5;
	}	
}
