#include "Player.h"
#include "GameStates/Play.h"
#include "GameTime.h"
#include "IOHandler.h"

Player::Player() : Enemy() {
	textureMap = {28, 52, 48,36,40, 26};
	charSpeed = playerSpeed;
	textureRef = textureMap.going;
}

void Player::addPlayer(Vector2DInt position) {
	Player* newPlayer = new Player();
	newPlayer->pos = { (float) position.x, (float) position.y };
	newPlayer->prevPos = newPlayer->pos;
	Enemy::player.reset(newPlayer);
}

void Player::findPath() {
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
		if (abs(enemy->pos.x - pos.x) < 0.5f && abs(enemy->pos.y - pos.y) < 0.5f) {
			die();
		}
	}
}

void Player::freeRun() {
	//Check digging input
	if (leftDigButton.impulse()) {
		if (brickO[curX - 1][curY - 1] && brickO[curX - 1][curY -1]->initiateDig()) {
			dPos.x = 0;
			dPos.y = 0;
			state = EnemyState::digging;
			direction = Direction::left;
			textureRef = 24;
			return;
		}
	}
	
	if (rightDigButton.impulse()) {
		if (brickO[curX + 1][curY - 1] && brickO[curX + 1][curY - 1]->initiateDig()) {
			dPos.x = 0;
			dPos.y = 0;
			state = EnemyState::digging;
			direction = Direction::right;
			textureRef = 25;
			return;
		}
	}

	Enemy::freeRun();
}

void Player::initiateFallingStart() {
	Enemy::initiateFallingStart();
	Audio::sfx[17].playPause();
}

void Player::initiateFallingStop() {
	Enemy::initiateFallingStop();
	Audio::sfx[17].stopAndRewind();
}

void Player::falling() {
	if (layout[curX][curY] == LayoutBlock::trapDoor) {
		trapdoors[curX][curY]->setRevealed();
	}

	Enemy::falling();

	if (state != EnemyState::falling) {
		idleTimeStart = GameTime::getGameTime();
	}
}

bool Player::checkHole() {
	return false;
}

void Player::digging() {
	//Position runner to middle and hold him in place
	if (pos.x > curX) {
		if (pos.x - actualSpeed < curX) {
			dPos.x = curX - pos.x;
		}
		else {
			dPos.x = -actualSpeed;
		}
	}
	else if (pos.x < curX) {
		if (pos.x + actualSpeed > curX) {
			dPos.x = curX - pos.x;
		}
		else {
			dPos.x = actualSpeed;
		}
	}
	else {
		dPos.x = 0;
	}

	if (pos.y > curY) {
		if (pos.y - actualSpeed < curY) {
			dPos.y = curY - pos.y;
		}
		else {
			dPos.y = -actualSpeed;
		}
	}
	else if (pos.y < curY) {
		if (pos.y + actualSpeed > curY) {
			dPos.y = curY - pos.y;
		}
		else {
			dPos.y = actualSpeed;
		}
	}
	else {
		dPos.y = 0;
	}
}

void Player::releaseFromDigging() {
	state = EnemyState::freeRun;
	idleTimeStart = gameTime;
}

void Player::animateFreeRun() {
	//last time of moving == start of idle time
	if (prevPos.x - pos.x != 0) {
		idleTimeStart = gameTime;
	}

	//runner idle animation
	if (pos.x - prevPos.x == 0 && pos.y - prevPos.y == 0 && gameTime - idleTimeStart > 1 && (middle != LayoutBlock::pole) && pos.y == curY) {
		textureRef = textureMap.idle + (int (gameTime)) % 2;
	}

	Enemy::animateFreeRun();
}

void Player::animateDigging() {
	if (middle == LayoutBlock::pole) {
		textureRef = textureMap.pole;
	}
	else if (middle == LayoutBlock::ladder) {
		textureRef = textureMap.ladder;
	}
}

void Player::animateDying() {
	
}

void Player::animateFalling() {
	Enemy::animateFalling();
}

void Player::animateGoing() {
	Enemy::animateGoing();

	if (Audio::sfx[9 + going[0]].getPlayStatus() == AudioStatus::stopped) {
		Audio::sfx[10 - going[0]].playPause();
		going[0] = 1 - going[0];
	}
}

void Player::animateOnLadder() {
	Enemy::animateOnLadder();

	if (Audio::sfx[11 + going[1]].getPlayStatus() == AudioStatus::stopped) {
		Audio::sfx[12 - going[1]].playPause();
		going[1] = 1 - going[1];
	}
}

void Player::animateOnPole() {
	Enemy::animateOnPole();

	if (Audio::sfx[15 + going[2]].getPlayStatus() == AudioStatus::stopped) {
		Audio::sfx[16 - going[2]].playPause();
		going[2] = 1 - going[2];
	}	
}

//player does not check gold rather top of level,
void Player::checkGoldCollect() {
	if ((carriedGold = Gold::goldCollectChecker(pos.x, pos.y))) {
		if (Audio::sfx[0].getPlayStatus() == AudioStatus::playing) {
			Audio::sfx[0].stopAndRewind();
		}

		Audio::sfx[0].playPause();
		Gold::addGoldToCollected(std::move(carriedGold));

		//if every gold is collected draw the ladders which are needed to finish the level
		if (!Enemy::hasGold() && Gold::getUncollectedSize() == 0) {
			Audio::sfx[4].playPause();
			play->generateFinishingLadders();
		}
	}
	
	//if every gold collected!
	if (Gold::getUncollectedSize() == 0 && !Enemy::hasGold()) {
		//top of level reached
		if (curY >= play->getHighestLadder() + 1) {
			play->transitionToOutro(killCounter, Gold::getCollectedSize(), 0);
			
			//score_gold = collectedsize * 200;
		}
	}
}

void Player::die() {
	dieTimer = GameTime::getCurrentFrame();
	play->transitionToDeath();
}

void Player::dying() {
	float deathLength = Audio::sfx[3].lengthInSec();
	int timeFactor = ((int) (9 * (GameTime::getCurrentFrame() - dieTimer) / deathLength)) % 9;
	timeFactor = (timeFactor == 8) ? 31 : timeFactor;

	textureRef = textureMap.death + timeFactor;
	Drawing::drawEnemy(pos.x, pos.y, textureRef, direction, false);
}