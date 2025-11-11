#ifndef BRICK_H
#define BRICK_H

#include "Vector2DInt.h"

enum class BrickState {
	digging,
	watiting,
	building,
	rebuilt
};

class Brick {
private:
	Vector2DInt position;
	float timer;
	BrickState brickState = BrickState::digging;

	const float diggingTime = 7.15f;
	const float destroyTime = 0.35f;
	const float buildTime = 0.5f;

	inline void digging(float);
	inline void waiting(float);
	inline void building(float);

	int textureState = 0;
public:
	Brick(Vector2DInt position, float gameTime);
	void handle(float);

	Vector2DInt getPosition() { return position; }

	friend class GameContext;
	friend class GameElements;
};

#endif // !BRICK_H
