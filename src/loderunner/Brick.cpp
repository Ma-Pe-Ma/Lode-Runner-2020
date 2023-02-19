#include "Brick.h"
#include "Enemy.h"
#include "Player.h"
#include "GameTime.h"
#include "Gold.h"

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
	int checkGold = gameContext->goldChecker(position.x, position.y + 1);
	LayoutBlock upBlock = gameContext->getLayout()[position.x][position.y + 1];

	if (brickState == BrickState::original && upBlock == LayoutBlock::empty && !checkGold) {
		brickState = BrickState::digging;
		timer = GameTime::getGameTime();

		Audio::sfx[1].stopAndRewind();
		Audio::sfx[1].playPause();
		gameContext->setRandomDebris(rand() % 3);

		return true;
	}
	
	return false;
}

void Brick::digging(float gameTime) {
	if (gameTime - timer > destroyTime) {
		brickState = BrickState::watiting;
		*pointerToTexture = 15;
		timer = gameTime;
		gameContext->notifyPlayerAboutDigEnd();
		gameContext->getLayout()[position.x][position.y] = LayoutBlock::empty;
		*(gameContext->getPointerToDebrisTexture()) = 15;
		gameContext->getPointerToDebrisLocation()[0] = -1;
		gameContext->getPointerToDebrisLocation()[1] = -1;
	}
	else {
		//growing hole
		int timeFactor = int(5 * (gameTime - timer) / (destroyTime)) % 5;	
		*pointerToTexture = 1 + timeFactor;

		if (gameContext->checkDigPrevention(position.x, position.y)) {
			Audio::sfx[2].playPause();
			Audio::sfx[1].stopAndRewind();
			gameContext->notifyPlayerAboutDigEnd();
			gameContext->getLayout()[position.x][position.y] = LayoutBlock::brick;
			brickState = BrickState::original;
			*pointerToTexture = 0;
			*(gameContext->getPointerToDebrisTexture()) = 15;
			gameContext->getPointerToDebrisLocation()[0] = -1;
			gameContext->getPointerToDebrisLocation()[1] = -1;
			return;
		}

		//drawing debris above hole
		if (gameContext->getLayout()[position.x][position.y + 1] != LayoutBlock::brick) {
			*(gameContext->getPointerToDebrisTexture()) = gameContext->getRandomDebris() * 6  + 19 + timeFactor;
			gameContext->getPointerToDebrisLocation()[0] = position.x;
			gameContext->getPointerToDebrisLocation()[1] = position.y + 1;
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

		gameContext->checkDeaths(position.x, position.y);
		gameContext->getLayout()[position.x][position.y] = LayoutBlock::brick;
	}
	else {
		int timeFactor = int(5 * (gameTime - timer) / buildTime) % 5;
		*pointerToTexture = 11 - timeFactor;
	}	
}

void Brick::setGameContext(std::shared_ptr<GameContext> gameContext)
{
	this->gameContext = gameContext;
}