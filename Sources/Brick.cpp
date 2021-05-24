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

void Brick::Handle(float gameTime) {
	switch (brickState)	{
	case original:
		DrawLevel(position.x, position.y, 0);
		break;
	case diggingS:
		Digging(gameTime);
		break;
	case watiting:
		Waiting(gameTime);
		break;
	case building:
		Building(gameTime);
		break;
	default:

		break;
	}
}

bool Brick::InitiateDig() {	
	int checkGold = Gold::GoldChecker(position.x, position.y + 1);
	LayoutBlock upBlock = layout[position.x][position.y + 1];

	if (brickState == original && upBlock == empty && !checkGold) {
		brickState = diggingS;
		timer = GameTime::getGameTime();

		Audio::SFX[1].StopAndRewind();
		Audio::SFX[1].PlayPause();
		randomDebris = rand() % 3;

		return true;
	}
	
	return false;
}

void Brick::Digging(float gameTime) {
	if (gameTime - timer > destroyTime) {
		brickState = watiting;
		timer = gameTime;
		Enemy::NotifyPlayerAboutDigEnd();
		layout[position.x][position.y] = empty;
	}
	else {
		//growing hole
		int timeFactor = int(5 * (gameTime - timer) / (destroyTime)) % 5;
		DrawLevel(position.x, position.y, 1 + timeFactor);

		if (Enemy::CheckDigPrevention(position.x, position.y)) {
			Audio::SFX[2].PlayPause();
			Audio::SFX[1].StopAndRewind();
			Enemy::NotifyPlayerAboutDigEnd();
			layout[position.x][position.y] = brick;
			brickState = original;
			DrawLevel(position.x, position.y, 0);
			return;
		}

		//drawing debris above hole
		if (layout[position.x][position.y + 1] != brick) {
			int debrisFactor = 19 + randomDebris * 6;
			DrawLevel(position.x, position.y + 1, debrisFactor + timeFactor);
		}
	}		
}

void Brick::Waiting(float gameTime) {
	if (gameTime - timer > diggingTime - destroyTime - buildTime) {
		brickState = building;
		timer = gameTime;
	}
}

void Brick::Building(float gameTime) {
	if (gameTime - timer > buildTime) {
		brickState = original;
		DrawLevel(position.x, position.y, 0);

		Enemy::CheckDeaths(position.x, position.y);
		layout[position.x][position.y] = brick;
	}
	else {
		int timeFactor = int(5 * (gameTime - timer) / buildTime) % 5;
		DrawLevel(position.x, position.y, 11 - timeFactor);
	}	
}