#include "Brick.h"

#include "Enemy.h"
#include "Player.h"
#include "Gold.h"

#include "iocontext/audio/AudioContext.h"
#include "iocontext/audio/AudioFile.h"

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

bool Brick::initiateDig(float gameTime) {	
	int checkGold = gameContext->goldChecker(position.x, position.y + 1);
	LayoutBlock upBlock = gameContext->getLayoutElement(position.x, position.y + 1);

	if (brickState == BrickState::original && upBlock == LayoutBlock::empty && !checkGold) {
		brickState = BrickState::digging;
		timer = gameTime;

		gameContext->getAudio()->getAudioFileByID(1)->stopAndRewind();
		gameContext->getAudio()->getAudioFileByID(1)->playPause();
		gameContext->setRandomDebris(rand() % 3);

		return true;
	}
	
	return false;
}

void Brick::digging(float gameTime) {
	if (gameTime - timer >= destroyTime) {
		brickState = BrickState::watiting;
		*pointerToTexture = 15;
		timer = gameTime;
		gameContext->notifyPlayerAboutDigEnd();
		gameContext->setLayoutElement(position.x, position.y, LayoutBlock::empty);
		*(gameContext->getPointerToDebrisTexture()) = 15;
		gameContext->getPointerToDebrisLocation()[0] = -1;
		gameContext->getPointerToDebrisLocation()[1] = -1;
	}
	else {
		//growing hole
		int timeFactor = int(5 * (gameTime - timer) / destroyTime) % 5;	
		*pointerToTexture = 1 + timeFactor;

		if (gameContext->checkDigPrevention(position.x, position.y)) {
			gameContext->getAudio()->getAudioFileByID(2)->playPause();
			gameContext->getAudio()->getAudioFileByID(1)->stopAndRewind();
			gameContext->notifyPlayerAboutDigEnd();
			gameContext->setLayoutElement(position.x, position.y, LayoutBlock::brick);
			brickState = BrickState::original;
			*pointerToTexture = 0;
			*(gameContext->getPointerToDebrisTexture()) = 15;
			gameContext->getPointerToDebrisLocation()[0] = -1;
			gameContext->getPointerToDebrisLocation()[1] = -1;
			return;
		}

		//drawing debris above hole
		if (gameContext->getLayoutElement(position.x, position.y + 1) != LayoutBlock::brick) {
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
	if (gameTime - timer >= buildTime) {
		brickState = BrickState::original;
		*pointerToTexture = 0;

		gameContext->checkDeaths(position.x, position.y);
		gameContext->setLayoutElement(position.x, position.y, LayoutBlock::brick);
	}
	else {
		int timeFactor = int(5 * (gameTime - timer) / buildTime) % 5;
		*pointerToTexture = 11 - timeFactor;
	}	
}

void Brick::setGameContext(GameContext* gameContext)
{
	this->gameContext = gameContext;
}