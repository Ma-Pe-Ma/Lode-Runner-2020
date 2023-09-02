#include "Player.h"
#include "GameStates/Play.h"
#include "iocontext/IOContext.h"

#include "Audio/AudioFile.h"

Player::Player(float x, float y) : Enemy(x, y) {
	this->textureMap = {28, 52, 48, 36, 40, 26};
}

void Player::setCharSpeed(float charSpeed) {
	this->charSpeed = charSpeed;
}

void Player::findPath() {
	if (ioContext->getRightButton().continuous() && ioContext->getLeftButton().continuous()) {
		dPos.x = 0;
	}
	else if (ioContext->getRightButton().continuous()) {
		dPos.x = actualSpeed;
	}
	else if (ioContext->getLeftButton().continuous()) {
		dPos.x = -actualSpeed;
	}

	if (ioContext->getUpButton().continuous() && ioContext->getDownButton().continuous()) {
		dPos.y = 0;
	}
	else if (ioContext->getUpButton().continuous()) {
		dPos.y = actualSpeed;
	}
	else if (ioContext->getDownButton().continuous()) {
		dPos.y = -actualSpeed;
	}

	gameContext->checkPlayerDeathByEnemy();
}

void Player::freeRun() {
	//Check digging input
	if (ioContext->getLeftDigButton().impulse()) {
		if (gameContext->getBrickByCoordinates(current.x - 1, current.y - 1) && gameContext->getBrickByCoordinates(current.x - 1, current.y -1)->initiateDig(gameTime)) {
			dPos.x = 0;
			dPos.y = 0;
			state = EnemyState::digging;
			direction = Direction::left;
			*texturePointer = 24;
			return;
		}
	}
	
	if (ioContext->getRightDigButton().impulse()) {
		if (gameContext->getBrickByCoordinates(current.x + 1, current.y - 1) && gameContext->getBrickByCoordinates(current.x + 1, current.y - 1)->initiateDig(gameTime)) {
			dPos.x = 0;
			dPos.y = 0;
			state = EnemyState::digging;
			direction = Direction::right;
			*texturePointer = 25;
			return;
		}
	}

	Enemy::freeRun();
}

void Player::initiateFallingStart() {
	Enemy::initiateFallingStart();
	gameContext->getAudio()->getAudioFileByID(17)->playPause();
}

void Player::initiateFallingStop() {
	Enemy::initiateFallingStop();
	gameContext->getAudio()->getAudioFileByID(17)->stopAndRewind();
}

void Player::falling() {
	if (gameContext->getLayoutElement(current.x, current.y) == LayoutBlock::trapDoor) {
		gameContext->getTrapdoorByCoordinate(current.x, current.y)->setRevealed();
	}

	Enemy::falling();

	if (state != EnemyState::falling) {
		idleTimeStart = gameTime;
	}
}

bool Player::checkHole() {
	return false;
}

void Player::digging() {
	//Position runner to middle and hold him in place
	if (pos.x > current.x) {
		if (pos.x - actualSpeed < current.x) {
			dPos.x = current.x - pos.x;
		}
		else {
			dPos.x = -actualSpeed;
		}
	}
	else if (pos.x < current.x) {
		if (pos.x + actualSpeed > current.x) {
			dPos.x = current.x - pos.x;
		}
		else {
			dPos.x = actualSpeed;
		}
	}
	else {
		dPos.x = 0;
	}

	if (pos.y > current.y) {
		if (pos.y - actualSpeed < current.y) {
			dPos.y = current.y - pos.y;
		}
		else {
			dPos.y = -actualSpeed;
		}
	}
	else if (pos.y < current.y) {
		if (pos.y + actualSpeed > current.y) {
			dPos.y = current.y - pos.y;
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
	if (pos.x - prevPos.x == 0 && pos.y - prevPos.y == 0 && gameTime - idleTimeStart > 1 && (middle != LayoutBlock::pole) && pos.y == current.y) {
		*texturePointer = textureMap.idle + (int (gameTime)) % 2;
	}

	Enemy::animateFreeRun();
}

void Player::animateDigging() {
	if (middle == LayoutBlock::pole) {
		*texturePointer = textureMap.pole;
	}
	else if (middle == LayoutBlock::ladder) {
		*texturePointer = textureMap.ladder;
	}
}

void Player::animateGoing() {
	Enemy::animateGoing();

	if (gameContext->getAudio()->getAudioFileByID(9 + going[0])->getPlayStatus() == AudioStatus::stopped) {
		gameContext->getAudio()->getAudioFileByID(10 - going[0])->playPause();
		going[0] = 1 - going[0];
	}
}

void Player::animateOnLadder() {
	Enemy::animateOnLadder();

	if (gameContext->getAudio()->getAudioFileByID(11 + going[1])->getPlayStatus() == AudioStatus::stopped) {
		gameContext->getAudio()->getAudioFileByID(12 - going[1])->playPause();
		going[1] = 1 - going[1];
	}
}

void Player::animateOnPole() {
	Enemy::animateOnPole();

	if (gameContext->getAudio()->getAudioFileByID(15 + going[2])->getPlayStatus() == AudioStatus::stopped) {
		gameContext->getAudio()->getAudioFileByID(16 - going[2])->playPause();
		going[2] = 1 - going[2];
	}	
}

void Player::checkGoldCollect() {
	if ((carriedGold = gameContext->goldCollectChecker(pos.x, pos.y))) {
		if (gameContext->getAudio()->getAudioFileByID(0)->getPlayStatus() == AudioStatus::playing) {
			gameContext->getAudio()->getAudioFileByID(0)->stopAndRewind();
		}

		gameContext->getAudio()->getAudioFileByID(0)->playPause();
		gameContext->addGoldToCollectedList(carriedGold);

		//if every gold is collected draw the ladders which are needed to finish the level
		if (!gameContext->enemyCarriesGold() && gameContext->getUncollectedGoldSize() == 0) {
			gameContext->getAudio()->getAudioFileByID(4)->playPause();
			gameContext->generateFinishingLadders();
		}
	}
	
	//if every gold collected!
	if (gameContext->getUncollectedGoldSize() == 0 && !gameContext->enemyCarriesGold()) {
		//top of level reached
		if (current.y >= gameContext->getHighestLadder() + 1) {
			gameContext->transitionToOutro();
		}
	}
}

void Player::die() {
	gameTime = 0.0f;
	gameContext->transitionToDeath();
}

void Player::dying() {
	float deathLength = gameContext->getAudio()->getAudioFileByID(3)->lengthInSec();
	int timeFactor = ((int)(9 * (gameTime - dieTimer) / deathLength)) % 9;
	timeFactor = (timeFactor == 8) ? 31 : timeFactor;
	*texturePointer = textureMap.death + timeFactor;
}
