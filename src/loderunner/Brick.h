#ifndef BRICK_H
#define BRICK_H

#include "Drawing.h"
#include "Structs/Vector2DInt.h"
#include "Enums/LayoutBlock.h"

enum class BrickState {
	original,
	digging,
	watiting,
	building
};

class Brick {
private:
	static std::shared_ptr<Brick>** brick;
	static LayoutBlock** layout;
	Vector2DInt position;
	float timer;
	BrickState brickState = BrickState::original;

	const float diggingTime = 7.15f;
	const float destroyTime = 0.5f;
	const float buildTime = 0.5f;

	inline void digging(float);
	inline void waiting(float);
	inline void building(float);

	static int randomDebris;
	
	static int* pointerToDebrisTexture;
	static int* pointerToDebrisLocation;

	int* pointerToTexture;
public:
	Brick(Vector2DInt);
	void handle(float);
	bool initiateDig();
	static void setLayoutPointers(LayoutBlock** layout, std::shared_ptr<Brick>**);
	Vector2DInt getPosition() { return position; }

	void setTexturePointer(int* pointerToTexture)
	{
		this->pointerToTexture = pointerToTexture;
	}

	static void setPointerToDebrisTexture(int*);
	static void setPointerToDebrisLocation(int*);
};

#endif // !BRICK_H
