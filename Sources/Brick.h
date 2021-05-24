#ifndef BRICK_H
#define BRICK_H

#include "Drawing.h"
#include "Structs/Vector2DInt.h"
#include "Enums/LayoutBlock.h"

enum BrickState {
	original,
	diggingS,
	watiting,
	building
};

class Brick {
private:
	static std::unique_ptr<Brick>** brickO;
	static LayoutBlock** layout;
	Vector2DInt position;
	float timer;
	BrickState brickState = original;

	const float diggingTime = 7.15f;
	const float destroyTime = 0.5f;
	const float buildTime = 0.5f;

	inline void Digging(float);
	inline void Waiting(float);
	inline void Building(float);

	static int randomDebris;
public:
	Brick(Vector2DInt);
	void Handle(float);
	bool InitiateDig();
	static void setLayoutPointers(LayoutBlock** layout, std::unique_ptr<Brick>**);
	Vector2DInt getPosition() { return position; }
};

#endif // !BRICK_H
