#include "Brick.h"
#include "Enemy.h"
#include "Player.h"
#include "GameTime.h"

int Brick::randomDebris = 0;
std::unique_ptr<Brick>** Brick::brickO;
LayoutBlock** Brick::layout;

void Brick::setLayoutPointers(LayoutBlock** layout, std::unique_ptr<Brick>** brickO) {
	Brick::layout = layout;
	Brick::brickO = brickO;
}

Brick::Brick(Vector2DInt position) {
	this->position = position;
}

void Brick::handle(float gameTime) {
	switch (brickState)	{
	case BrickState::original:
		Drawing::drawLevel(position.x, position.y, 0);
		break;
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

	if (brickState == original && upBlock == LayoutBlock::empty && !checkGold) {
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
		timer = gameTime;
		Enemy::notifyPlayerAboutDigEnd();
		layout[position.x][position.y] = LayoutBlock::empty;
	}
	else {
		//growing hole
		int timeFactor = int(5 * (gameTime - timer) / (destroyTime)) % 5;
		Drawing::drawLevel(position.x, position.y, 1 + timeFactor);

		if (Enemy::checkDigPrevention(position.x, position.y)) {
			Audio::sfx[2].playPause();
			Audio::sfx[1].stopAndRewind();
			Enemy::notifyPlayerAboutDigEnd();
			layout[position.x][position.y] = LayoutBlock::brick;
			brickState = BrickState::original;
			Drawing::drawLevel(position.x, position.y, 0);
			return;
		}

		//drawing debris above hole
		if (layout[position.x][position.y + 1] != LayoutBlock::brick) {
			int debrisFactor = 19 + randomDebris * 6;
			Drawing::drawLevel(position.x, position.y + 1, debrisFactor + timeFactor);
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
		Drawing::drawLevel(position.x, position.y, 0);

		Enemy::checkDeaths(position.x, position.y);
		layout[position.x][position.y] = LayoutBlock::brick;
	}
	else {
		int timeFactor = int(5 * (gameTime - timer) / buildTime) % 5;
		Drawing::drawLevel(position.x, position.y, 11 - timeFactor);
	}	
}