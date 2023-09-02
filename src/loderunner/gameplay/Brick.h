#ifndef BRICK_H
#define BRICK_H

#include "Vector2DInt.h"
#include "LayoutBlock.h"

#include <memory>

class GameContext;

enum class BrickState {
	original,
	digging,
	watiting,
	building
};

class Brick {
private:
	Vector2DInt position;
	float timer;
	BrickState brickState = BrickState::original;

	const float diggingTime = 7.15f;
	const float destroyTime = 0.5f;
	const float buildTime = 0.5f;

	inline void digging(float);
	inline void waiting(float);
	inline void building(float);

	int* pointerToTexture;

	GameContext* gameContext;
public:
	Brick(Vector2DInt);
	void handle(float);
	bool initiateDig(float);
	Vector2DInt getPosition() { return position; }

	void setTexturePointer(int* pointerToTexture)
	{
		this->pointerToTexture = pointerToTexture;
	}

	void setGameContext(GameContext* gameContext);
};

#endif // !BRICK_H
