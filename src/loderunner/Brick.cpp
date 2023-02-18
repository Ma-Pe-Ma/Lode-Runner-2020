#include "Brick.h"
#include "Enemy.h"
#include "Player.h"
#include "GameTime.h"
#include "Gold.h"

int Brick::randomDebris = 0;

int* Brick::pointerToDebrisTexture;
int* Brick::pointerToDebrisLocation;

std::shared_ptr<Brick>** Brick::brick;
LayoutBlock** Brick::layout;

void Brick::setPointerToDebrisTexture(int* pointerToDebrisTexture)
{
	Brick::pointerToDebrisTexture = pointerToDebrisTexture;
}

void Brick::setPointerToDebrisLocation(int* pointerToDebrisLocation)
{
	Brick::pointerToDebrisLocation = pointerToDebrisLocation;
}

void Brick::setLayoutPointers(LayoutBlock** layout, std::shared_ptr<Brick>** brick) {
	Brick::layout = layout;
	Brick::brick = brick;
}

Brick::Brick(Vector2DInt position) {
	this->position = position;
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

bool Brick::initiateDig() {	
	int checkGold = Gold::goldChecker(position.x, position.y + 1);
	LayoutBlock upBlock = layout[position.x][position.y + 1];

	if (brickState == BrickState::original && upBlock == LayoutBlock::empty && !checkGold) {
		brickState = BrickState::digging;
		timer = GameTime::getGameTime();

		Audio::sfx[1].stopAndRewind();
		Audio::sfx[1].playPause();
		randomDebris = rand() % 3;

		return true;
	}
	
	return false;
}

void Brick::digging(float gameTime) {
	if (gameTime - timer > destroyTime) {
		brickState = BrickState::watiting;
		*pointerToTexture = 15;
		timer = gameTime;
		Enemy::notifyPlayerAboutDigEnd();
		layout[position.x][position.y] = LayoutBlock::empty;
		*pointerToDebrisTexture = 15;
		pointerToDebrisLocation[0] = -1;
		pointerToDebrisLocation[1] = -1;
	}
	else {
		//growing hole
		int timeFactor = int(5 * (gameTime - timer) / (destroyTime)) % 5;	
		*pointerToTexture = 1 + timeFactor;

		if (Enemy::checkDigPrevention(position.x, position.y)) {
			Audio::sfx[2].playPause();
			Audio::sfx[1].stopAndRewind();
			Enemy::notifyPlayerAboutDigEnd();
			layout[position.x][position.y] = LayoutBlock::brick;
			brickState = BrickState::original;
			*pointerToTexture = 0;
			*pointerToDebrisTexture = 15;
			pointerToDebrisLocation[0] = -1;
			pointerToDebrisLocation[1] = -1;
			return;
		}

		//drawing debris above hole
		if (layout[position.x][position.y + 1] != LayoutBlock::brick) {
			*pointerToDebrisTexture = randomDebris * 6  + 19 + timeFactor;
			pointerToDebrisLocation[0] = position.x;
			pointerToDebrisLocation[1] = position.y + 1;
		}
	}		
}

void Brick::waiting(float gameTime) {
	if (gameTime - timer > diggingTime - destroyTime - buildTime) {
		brickState = BrickState::building;
		timer = gameTime;
	}
}

void Brick::building(float gameTime) {
	if (gameTime - timer > buildTime) {
		brickState = BrickState::original;
		*pointerToTexture = 0;

		Enemy::checkDeaths(position.x, position.y);
		layout[position.x][position.y] = LayoutBlock::brick;
	}
	else {
		int timeFactor = int(5 * (gameTime - timer) / buildTime) % 5;
		*pointerToTexture = 11 - timeFactor;
	}	
}