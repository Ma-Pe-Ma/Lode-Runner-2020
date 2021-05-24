#include "Player.h"
#include "GameStates/Play.h"
#include "GameTime.h"
#include "IOHandler.h"

Player::Player() : Enemy() {
	textureMap = {28, 52, 48,36,40, 26};
	charSpeed = playerSpeed;
	TextureRef = textureMap.going;
}

void Player::AddPlayer(Vector2DInt position) {
	Player* newPlayer = new Player();
	newPlayer->Pos = { (float) position.x, (float) position.y };
	newPlayer->prevPos = newPlayer->Pos;
	Enemy::player.reset(newPlayer);
}

void Player::FindPath() {
	actualSpeed = charSpeed * GameTime::getSpeed();

	if (rightButton.continuous() && leftButton.continuous()) {
		dPos.x = 0;
	}
	else if (rightButton.continuous()) {
		dPos.x = actualSpeed;
	}
	else if (leftButton.continuous()) {
		dPos.x = -actualSpeed;
	}

	if (up.continuous() && down.continuous()) {
		dPos.y = 0;
	}
	else if (up.continuous()) {
		dPos.y = actualSpeed;
	}
	else if (down.continuous()) {
		dPos.y = -actualSpeed;
	}

	//check if runner dies by enemy
	for (auto& enemy : enemies) {
		if (abs(enemy->Pos.x - Pos.x) < 0.5f && abs(enemy->Pos.y - Pos.y) < 0.5f) {
			Die();
		}
	}
}

void Player::FreeRun() {
	//Check digging input
	if (leftDigButton.impulse()) {
		if (brickO[curX - 1][curY - 1] && brickO[curX - 1][curY -1]->InitiateDig()) {
			dPos.x = 0;
			dPos.y = 0;

			state = digging;
			direction = left;
			TextureRef = 24;
			return;
		}
	}
	
	if (rightDigButton.impulse()) {
		if (brickO[curX + 1][curY - 1] && brickO[curX + 1][curY - 1]->InitiateDig()) {
			dPos.x = 0;
			dPos.y = 0;
			state = digging;
			direction = right;
			TextureRef = 25;
			return;
		}
	}

	Enemy::FreeRun();
}

void Player::InitiateFallingStart() {
	Enemy::InitiateFallingStart();
	Audio::SFX[17].PlayPause();
}

void Player::InitiateFallingStop() {
	Enemy::InitiateFallingStop();
	Audio::SFX[17].StopAndRewind();
}

void Player::Falling() {
	if (layout[curX][curY] == trapDoor) {
		trapdoors[curX][curY]->setRevealed();
	}

	Enemy::Falling();

	if (state != falling) {
		idleTimeStart = GameTime::getGameTime();
	}
}

bool Player::CheckHole() {
	return false;
}

void Player::Digging() {
	//Position runner to middle and hold him in place
	if (Pos.x > curX) {
		if (Pos.x - actualSpeed < curX) {
			dPos.x = curX - Pos.x;
		}
		else {
			dPos.x = -actualSpeed;
		}
	}
	else if (Pos.x < curX) {
		if (Pos.x + actualSpeed > curX) {
			dPos.x = curX - Pos.x;
		}
		else {
			dPos.x = actualSpeed;
		}
	}
	else {
		dPos.x = 0;
	}

	if (Pos.y > curY) {
		if (Pos.y - actualSpeed < curY) {
			dPos.y = curY - Pos.y;
		}
		else {
			dPos.y = -actualSpeed;
		}
	}
	else if (Pos.y < curY) {
		if (Pos.y + actualSpeed > curY) {
			dPos.y = curY - Pos.y;
		}
		else {
			dPos.y = actualSpeed;
		}
	}
	else {
		dPos.y = 0;
	}
}

void Player::ReleaseFromDigging() {
	state = freeRun;
	idleTimeStart = gameTime;
}

void Player::AnimateFreeRun() {
	//last time of moving == start of idle time
	if (prevPos.x - Pos.x != 0) {
		idleTimeStart = gameTime;
	}

	//runner idle animation
	if (Pos.x - prevPos.x == 0 && Pos.y - prevPos.y == 0 && gameTime - idleTimeStart > 1 && (middle != pole) && Pos.y == curY) {
		TextureRef = textureMap.idle + (int (gameTime)) % 2;
	}

	Enemy::AnimateFreeRun();
}

void Player::AnimateDigging() {
	if (middle == pole) {
		TextureRef = textureMap.pole;
	}
	else if (middle == ladder) {
		TextureRef = textureMap.ladder;
	}
}

void Player::AnimateDying() {
	
}

void Player::AnimateFalling() {
	Enemy::AnimateFalling();
}

void Player::AnimateGoing() {
	Enemy::AnimateGoing();

	if (Audio::SFX[9 + going[0]].GetPlayStatus() == stopped) {
		Audio::SFX[10 - going[0]].PlayPause();
		going[0] = 1 - going[0];
	}
}

void Player::AnimateOnLadder() {
	Enemy::AnimateOnLadder();

	if (Audio::SFX[11 + going[1]].GetPlayStatus() == stopped) {
		Audio::SFX[12 - going[1]].PlayPause();
		going[1] = 1 - going[1];
	}
}

void Player::AnimateOnPole() {
	Enemy::AnimateOnPole();

	if (Audio::SFX[15 + going[2]].GetPlayStatus() == stopped) {
		Audio::SFX[16 - going[2]].PlayPause();
		going[2] = 1 - going[2];
	}	
}

//player does not check gold rather top of level,
void Player::CheckGoldCollect() {
	if ((carriedGold = Gold::GoldCollectChecker(Pos.x, Pos.y))) {
		if (Audio::SFX[0].GetPlayStatus() == playing) {
			Audio::SFX[0].StopAndRewind();
		}

		Audio::SFX[0].PlayPause();
		Gold::addGoldToCollected(std::move(carriedGold));

		//if every gold is collected draw the ladders which are needed to finish the level
		if (!Enemy::HasGold() && Gold::GetUncollectedSize() == 0) {
			Audio::SFX[4].PlayPause();
			play->generateFinishingLadders();
		}
	}
	
	//if every gold collected!
	if (Gold::GetUncollectedSize() == 0 && !Enemy::HasGold()) {
		//top of level reached
		if (curY >= play->getHighestLadder() + 1) {
			play->TransitionToOutro(killCounter, Gold::GetCollectedSize(), 0);
			
			//score_gold = collectedsize * 200;
		}
	}
}

void Player::Die() {
	dieTimer = GameTime::getCurrentFrame();
	play->TransitionToDeath();
}

void Player::Dying() {
	float deathLength = Audio::SFX[3].LengthInSec();
	int timeFactor = ((int) (9 * (GameTime::getCurrentFrame() - dieTimer) / deathLength)) % 9;
	timeFactor = (timeFactor == 8) ? 31 : timeFactor;

	TextureRef = textureMap.death + timeFactor;
	DrawEnemy(Pos.x, Pos.y, TextureRef, direction, false);
}