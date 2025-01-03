#include "Enemy.h"

#include "Player.h"
#include "Gold.h"

#include "iocontext/audio/AudioFile.h"

#include "states/gamestates/Play.h"

#include <cmath>

Enemy::Enemy(float x, float y) {
	this->pos = { x, y };
	this->prevPos = this->pos;
	this->dPos = { 0.0f, 0.0f };
	this->dPrevPos = { 0.0f, 0.0f };

	direction = Direction::right;
	pitState = PitState::fallingToPit;
	state = EnemyState::freeRun;

	carriedGold = nullptr;
}

void Enemy::checkDeath(int x, int y) {
	if (std::abs(pos.x - x) < 0.5f && y - 0.5f <= pos.y && pos.y < y + 0.75f) {

		if (state == EnemyState::falling || pitState == PitState::fallingToPit)
		{
			if (carriedGold && gameContext->getBrickByCoordinates(x, y + 1) == nullptr)
			{
				carriedGold->setPos({ x, y + 1 });
				gameContext->addGoldToUncollectedList(carriedGold);
				carriedGold = nullptr;
			}
		}
		
		die();
	}
}

bool Enemy::enemyChecker(float x, float y) {
	for (auto& enemy : gameContext->getEnemies()) {
		if (enemy.get() != this) {
			if (std::abs(x - enemy->pos.x) < 0.5f && std::abs(y - enemy->pos.y) < 0.5f) {
				return true;
			}
		}
	}

	return false;
}

void Enemy::handle(float gameTime, float frameDelta) {
	this->gameTime = gameTime;
	this->frameDelta = frameDelta;
	this->actualSpeed = charSpeed * frameDelta * 5.0f;	

	determineNearbyObjects();
	findPath();
	move();

	pos = { pos.x + dPos.x, pos.y + dPos.y };

	positionPointer[0] = pos.x;
	positionPointer[1] = pos.y;

	if (prevPos.x != pos.x && state != EnemyState::digging) {
		if (pos.x - prevPos.x > 0) {
			direction = Direction::right;
		}
		else if (pos.x - prevPos.x < 0) {
			direction = Direction::left;
		}
	}

	dPos = { 0.0f, 0.0f };

	animate();

	*directionPointer = direction == Direction::left;
	*carryGoldPointer = carriedGold != nullptr;

	dPrevPos = { pos.x - prevPos.x, pos.y - prevPos.y };
	prevPos = { pos.x, pos.y };
}

void Enemy::determineNearbyObjects() {
	current.x = int(pos.x + 0.5);
	current.y = int(pos.y + 0.5);

	middle = gameContext->getLayoutElement(current.x, current.y);
	downBlock = gameContext->getLayoutElement(current.x, current.y - 1);
}

//-----------------------------------------------------------
//----------------------PATH FINDING-------------------------

void Enemy::findPath() {
	//reminder for implementing the moving from the other document
	//base is untouched - ie. it is the same as the text file
	//golds are in .base 
	//enemies are in .act and not in base!
	//if a hole is created then the value of brick in act is emptied, but in the base it is unchanged!

#ifndef NDEBUG
	if (*gameContext->getGameConfiguration()->getEnemyDebugState()) {
		auto& buttonInputs = gameContext->getIOContext()->getButtonInputs();

		if (debugEnemy == 1)
		{
			if (buttonInputs.d1Right.continuous()) {
				dPos.x = actualSpeed;
			}
			else if (buttonInputs.d1Left.continuous()) {
				dPos.x = -actualSpeed;
			}

			if (buttonInputs.d1Up.continuous()) {
				dPos.y = actualSpeed;
			}
			else if (buttonInputs.d1Down.continuous()) {
				dPos.y = -actualSpeed;
			}
		}
		else if (debugEnemy == 2)
		{
			auto io = gameContext->getIOContext();

			if (buttonInputs.d2Right.continuous()) {
				dPos.x = actualSpeed;
			}
			else if (buttonInputs.d2Left.continuous()) {
				dPos.x = -actualSpeed;
			}

			if (buttonInputs.d2Up.continuous()) {
				dPos.y = actualSpeed;
			}
			else if (buttonInputs.d2Down.continuous()) {
				dPos.y = -actualSpeed;
			}
		}

		return;
	}

#endif

	int x = current.x;
	int y = current.y;

	Vector2DInt runnerPos = gameContext->getPlayer()->current;
	
	//while
	if (y == runnerPos.y && gameContext->getPlayer()->state != EnemyState::falling) {
		while (x != runnerPos.x) {
			//std::cout << "\n x != runnerx";
			LayoutBlock checkable = gameContext->getLayoutElement(x, y);
			LayoutBlock belowCheckable = gameContext->getLayoutElement(x, y - 1);

			bool checkGold = gameContext->goldChecker(x, y - 1);

			if (checkable == LayoutBlock::ladder || checkable == LayoutBlock::pole || /*belowCheckable == LayoutBlock::brick ||*/ gameContext->getBrickByCoordinates(x, y - 1) || belowCheckable == LayoutBlock::concrete
				|| belowCheckable == LayoutBlock::ladder	|| enemyChecker(x, y - 1) || belowCheckable == LayoutBlock::pole || checkGold) {

				//guard left to runner
				if (x < runnerPos.x) {
					++x;
				}					

				//guard right to runner
				else if (x > runnerPos.x) {
					--x;
				}					
			}
			else {
				break;						 // exit loop with closest x if no path to runner
			}				
		}

		//scan for a path ignoring walls is a success
		if (x == runnerPos.x) {
			if (pos.x < runnerPos.x) {
				dPos.x = actualSpeed;
				dPos.y = 0;
			}
			else if (pos.x > runnerPos.x) {
				dPos.x = -actualSpeed;
				dPos.y = 0;
			}
			else {
				//this conditions are not necessary due to different implementations
				//if (player->dPos.x < 0) {
					//dPos.x = -actualSpeed;
				//}
				//else {
					//dPos.x = actualSpeed;
				//}
			}
			return;
		}
	}

	scanFloor();
}

void Enemy::scanFloor() {
	int startX = current.x;
	//int startY = current.y;

	int x = current.x;
	int y = current.y;

	bestPath = 0;
	bestRating = 255;

	while (x > 0) {                                    //get left end first
		LayoutBlock checkable = gameContext->getLayoutElement(x - 1, y);
		LayoutBlock belowCheckable = gameContext->getLayoutElement(x - 1, y - 1);

		if (checkable == LayoutBlock::brick || checkable == LayoutBlock::concrete) {
			break;
		}

		if (checkable == LayoutBlock::ladder || checkable == LayoutBlock::pole /*|| belowCheckable == brick*/	|| gameContext->getBrickByCoordinates(x - 1, y - 1) || belowCheckable == LayoutBlock::concrete || belowCheckable == LayoutBlock::ladder) {
			--x;
		}		
		else {
			--x;                                        // go on left anyway 
			break;
		}
	}

	//left-most possible location for enemy
	int leftEnd = x;

	x = startX;
	while (x < 29) {									// get right end next
		LayoutBlock checkable = gameContext->getLayoutElement(x + 1, y);;
		LayoutBlock belowCheckable = gameContext->getLayoutElement(x + 1, y - 1);

		if (checkable == LayoutBlock::brick || checkable == LayoutBlock::concrete) {
			break;
		}			
		if (checkable == LayoutBlock::ladder || checkable == LayoutBlock::pole /*|| belowCheckable == brick */ || gameContext->getBrickByCoordinates(x + 1, y - 1) || belowCheckable == LayoutBlock::concrete || belowCheckable == LayoutBlock::ladder) {
			++x;
		}			
		else {                                         // go on right anyway
			++x;
			break;
		}
	}

	int rightEnd = x;

	x = startX;
	LayoutBlock checkable = gameContext->getLayoutElement(x, y);
	LayoutBlock belowCheckable = gameContext->getLayoutElement(x, y - 1);

	//the possible directions (+0: stay), this numbers are the second arguments of the scandown and scanup functions (for the best path)
	//	1: left
	//	2: right
	//  3: up
	//  4: down

	if (/*belowCheckable != brick &&*/ !gameContext->getBrickByCoordinates(x, y - 1) && belowCheckable != LayoutBlock::concrete) {
		scanDown(x, 4);
	}		
	if (checkable == LayoutBlock::ladder) {
		scanUp(x, 3);
	}

	//first checking left direction, if curpath is 1 then we are checking the left side of the enemy, if 2, then the right side
	int curPath = 1;
	x = leftEnd;

	while (true) {
		if (x == startX) {
			if (curPath == 1 && rightEnd != startX) {
				curPath = 2;
				x = rightEnd;
			}
			else {
				break;
			}				
		}

		checkable = gameContext->getLayoutElement(x, y);
		belowCheckable = gameContext->getLayoutElement(x, y - 1);

		if (/*belowCheckable != brick &&*/ !gameContext->getBrickByCoordinates(x, y - 1) && belowCheckable != LayoutBlock::concrete) {
			scanDown(x, curPath);
		}

		if (checkable == LayoutBlock::ladder) {
			scanUp(x, curPath);
		}			

		if (curPath == 1) {
			x++;
		}			
		else {
			x--;
		}			
	}

	switch (bestPath) {
	case 1:
		dPos.x = -actualSpeed;
		dPos.y = 0;
		break;

	case 2:
		dPos.x = actualSpeed;
		dPos.y = 0;
		break;
	case 3:
		dPos.x = 0;
		dPos.y = actualSpeed;
		break;

	case 4:
		dPos.x = 0;
		dPos.y = -actualSpeed;
		break;

	default:
		dPos.x = 0;
		dPos.y = 0;
		break;
	}
}

void Enemy::scanDown(int x, int curPath) {
	int y = current.y;
	int runnerY = gameContext->getPlayer()->current.y;

	LayoutBlock belowCheckable = gameContext->getLayoutElement(x, y - 1);

	while (y > 0 && (belowCheckable = gameContext->getLayoutElement(x, y - 1)) != LayoutBlock::brick && !gameContext->getBrickByCoordinates(x, y - 1) && belowCheckable != LayoutBlock::concrete) {
		bool checkGold = gameContext->goldChecker(x, y);

		//here is a problem, with champ 19?
		if (gameContext->getLayoutElement(x, y) != LayoutBlock::empty || checkGold || gameContext->getBrickByCoordinates(x, y)) {
			if (x > 1) {
				LayoutBlock leftSideBelow = gameContext->getLayoutElement(x - 1, y - 1);
				LayoutBlock leftSide = gameContext->getLayoutElement(x - 1, y);

				if (leftSideBelow == LayoutBlock::brick || gameContext->getBrickByCoordinates(x - 1, y - 1) || leftSideBelow == LayoutBlock::concrete || leftSideBelow == LayoutBlock::ladder || leftSide == LayoutBlock::pole) {
					if (y <= runnerY) {
						break;
					}
				}
			}

			if (x < 28) {
				LayoutBlock rightSideBelow = gameContext->getLayoutElement(x + 1, y - 1);
				LayoutBlock rightSide = gameContext->getLayoutElement(x + 1, y);

				if (rightSideBelow == LayoutBlock::brick || gameContext->getBrickByCoordinates(x + 1, y - 1) || rightSideBelow == LayoutBlock::concrete || rightSideBelow == LayoutBlock::ladder || rightSide == LayoutBlock::pole) {
					if (y <= runnerY) {
						break;
					}
				}
			}
		}
		--y;
	}

	int curRating = 255;

	if (y == runnerY) {
		curRating = std::abs(current.x - x);				// update best rating and direct.
	}		
	else if (y < runnerY) {
		curRating = runnerY - y + 200;			// position below runner
	}		
	else {
		curRating = y - runnerY + 100;			// position above runner
	}		

	if (curRating < bestRating) {
		bestRating = curRating;
		bestPath = curPath;
	}
}

void Enemy::scanUp(int x, int curPath) {
	int y = current.y;
	int runnerY = gameContext->getPlayer()->current.y;

	while (y < 18 && gameContext->getLayoutElement(x, y) == LayoutBlock::ladder) {
		++y;

		LayoutBlock leftSideBelow = gameContext->getLayoutElement(x - 1, y - 1);
		LayoutBlock leftSide = gameContext->getLayoutElement(x - 1, y);

		LayoutBlock rightSideBelow = gameContext->getLayoutElement(x + 1, y - 1);
		LayoutBlock rightSide = gameContext->getLayoutElement(x + 1, y);

		if (x > 1) {
			if (/*leftSideBelow == brick ||*/ gameContext->getBrickByCoordinates(x - 1, y - 1) || leftSideBelow == LayoutBlock::concrete || leftSideBelow == LayoutBlock::ladder || leftSide == LayoutBlock::pole) {
				if (y >= runnerY) {
					break;
				}
			}
		}

		if (x < 28) {
			if (/*rightSideBelow == brick ||*/ gameContext->getBrickByCoordinates(x + 1, y - 1) || rightSideBelow == LayoutBlock::concrete || rightSideBelow == LayoutBlock::ladder || rightSide == LayoutBlock::pole) {
				if (y >= runnerY) {
					break;
				}
			}
		}
	}

	int curRating = 255;

	if (y == runnerY) {
		curRating = std::abs(current.x - x);			// update best rating and direct.
	}		
	else if (y < runnerY) {
		curRating = runnerY - y + 200;		// position below runner   
	}		
	else {
		curRating = y - runnerY + 100;		// position above runner    
	}		

	if (curRating < bestRating) {
		bestRating = curRating;
		bestPath = curPath;
	}
}

//-----------------------------------------------------------
//--------------------------MOVE-----------------------------

void Enemy::move() {
	switch (state) {
	case EnemyState::freeRun:
		freeRun();
		break;
	case EnemyState::startingToFall:
		startingToFall();
		break;
	case EnemyState::falling:
		falling();
		break;
	case EnemyState::digging:
		digging();
		break;
	case EnemyState::pitting:
		pitting();
		break;
	case EnemyState::dying:
		dying();
		break;
	}	
}

void Enemy::animate() {
	int factor = animationFactor * actualSpeed / frameDelta;
	animationTimeFactor = int(factor * gameTime) % 4;

	switch (state) {
	case EnemyState::freeRun:
		animateFreeRun();
		break;
	case EnemyState::startingToFall:
		animateFalling();
		break;
	case EnemyState::falling:
		animateFalling();
		break;
	case EnemyState::digging:
		animateDigging();
		break;
	case EnemyState::pitting:
		animatePitting();
		break;
	case EnemyState::dying:
		animateDying();
		break;
	}
}

void Enemy::startingToFall() {
	//falling back to pit, when enemy did not come out from it!
	if (pos.x == current.x) {
		dPos.x = 0;
		//std::cout << "\n exiting stf";
		state = EnemyState::falling;
		return;
	}

	//falling from edge left->right
	else if (pos.x < current.x) {
		direction = Direction::right;

		if (pos.x + actualSpeed > current.x) {
			dPos.x = current.x - pos.x;
			state = EnemyState::falling;
		}
		else {
			dPos.x = actualSpeed;
		}			
	}
	//falling from edge right->left
	else  {
		direction = Direction::left;

		if (pos.x - actualSpeed < current.x) {
			dPos.x = current.x - pos.x;
			state = EnemyState::falling;
		}
		else {
			dPos.x = -actualSpeed;
		}
	}

	dPos.y = -actualSpeed;
}

void Enemy::digging() {
	
}
//------FREERUN------

//To properly use ladder characters need to be adjusted into center!
void Enemy::ladderTransformation() {
	/*if (middle == ladder && dPos.y > 0) {
		dPos.y = 0;
	}*/

	//you can move on a ladder 1.5 unit vertically, middle block only covers the bottom 2/3 of the ladder, tophalfladder covers the upper 2/3 of ladder
	LayoutBlock topHalfOfLadder = gameContext->getLayoutElement(current.x, int(pos.y));

	if (dPos.y > 0 && middle != LayoutBlock::ladder && topHalfOfLadder != LayoutBlock::ladder) {
		dPos.y = 0;
		pos.y = int(pos.y + 0.5f);
	}

	if (dPos.y < 0 && middle != LayoutBlock::ladder && downBlock != LayoutBlock::ladder && middle != LayoutBlock::pole) {
		dPos.y = 0;
	}

	//Handle when multiple buttons pushed (only used by runner)
	if (dPos.x != 0 && dPos.y != 0) {
		if (dPos.y > 0) {
			if (middle == LayoutBlock::ladder) {
				dPos.x = 0;
			}
			else if (downBlock == LayoutBlock::ladder) {
				dPos.y = 0;
			}
		}
		else {
			if (middle == LayoutBlock::ladder) {
				if (downBlock == LayoutBlock::ladder || downBlock == LayoutBlock::pole || downBlock == LayoutBlock::empty || downBlock == LayoutBlock::trapDoor) {
					dPos.x = 0;
				}
				else {//if (downBlock != ladder) {
					dPos.y = 0;
				}
			}
			else {
				if (downBlock == LayoutBlock::ladder) {
					dPos.x = 0;
				}
				else {
					//dPos.x = 0;
				}
			}	
		}			
	}

	//adjust character to middle! (used by everyone)
	if (middle == LayoutBlock::ladder || downBlock == LayoutBlock::ladder) {
		if (dPos.x != 0 && pos.y != current.y) {
			dPos.x = 0;
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
		}
		else if (dPos.y != 0 && pos.x != current.x) {
			dPos.y = 0;
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
		}
	}
}

//enemy does not go into other enemy!
void Enemy::checkCollisionWithOthers() {
	bool checkPit = int(pos.x) != int(pos.x + dPos.x);

	for (auto& enemy : gameContext->getEnemies()) {
		if (enemy.get() != this) {
			//stop if other enemy under is moving!
			if (std::abs(enemy->pos.x - pos.x) < 1.0f && enemy->pos.y == pos.y - 1 && enemy->dPrevPos.x != 0 && middle != LayoutBlock::ladder && middle != LayoutBlock::pole && downBlock != LayoutBlock::ladder && enemy->state != EnemyState::pitting) {
				dPos.x = 0;
				return;
			}

			//check X collision
			if (dPos.x != 0 && std::abs(enemy->pos.x - (pos.x + dPos.x)) < 1.0f && std::abs(pos.y - enemy->pos.y) < 1.0f) {	
				//stop if enemy is coming out from hole
				if (checkPit) {
					if (enemy->state == EnemyState::pitting && enemy->pitState == PitState::climbing) {
						dPos.x = current.x - pos.x;
						break;
					}
				}

				//when going to same ladder from different sides, who goes from right to left, stop!
				if (-directionHelper.x == enemy->directionHelper.x) {
					if (directionHelper.x == -1) {
						dPos.x = 0;
					}
				}
				//do not go inside other enemy
				else {
					float enemyDiff = enemy->pos.x - pos.x;
					short enemyDirectionX = (enemyDiff > 0) - (enemyDiff < 0);
					if (enemyDirectionX == directionHelper.x) {
						dPos.x = 0;
					}
				}
			}

			//check Y collision
			if (dPos.y != 0 && std::abs(enemy->pos.y - (pos.y + dPos.y)) < 1.0f && std::abs(pos.x - enemy->pos.x) < 1.0f) {
				//when exiting the same ladder from different sides, who goes from up to down, stop!
				if (-directionHelper.y == enemy->directionHelper.y) {
					if (directionHelper.y == -1) {
						dPos.y = 0;
					}	
				}
				//do not go inside other enemy
				else {
					float enemyDiff = enemy->pos.y - pos.y;
					short enemyDirectionY = (enemyDiff > 0) - (enemyDiff < 0);
					if (enemyDirectionY == directionHelper.y) {
						dPos.y = 0;
					}
				}
			}
		}
	}
}

void Enemy::checkCollisionsWithEnvironment() {	
	if (dPos.x != 0) {
		short directionX = (dPos.x > 0) - (dPos.x < 0);
		int nextX = current.x + directionX;
		LayoutBlock nextXBlock = gameContext->getLayoutElement(nextX, current.y);

		if ((nextXBlock == LayoutBlock::brick || nextXBlock == LayoutBlock::concrete || nextXBlock == LayoutBlock::trapDoor) && std::abs(nextX - pos.x - dPos.x) < 1.0f) {
			dPos.x = current.x - pos.x;
		}
	}	

	if (dPos.y != 0) {
		short directionY = (dPos.y > 0) - (dPos.y < 0);
		int nextY = current.y + directionY;
		LayoutBlock nextYBlock = gameContext->getLayoutElement(current.x, nextY);

		if (dPos.y > 0) {
			if ((nextYBlock == LayoutBlock::brick || nextYBlock == LayoutBlock::concrete || nextYBlock == LayoutBlock::trapDoor) && std::abs(nextY - pos.y - dPos.y) < 1.0f) {
				dPos.y = current.y - pos.y;
			}
		}
		else {
			if ((nextYBlock == LayoutBlock::brick || nextYBlock == LayoutBlock::concrete) && std::abs(nextY - pos.y - dPos.y) < 1.0f) {
				dPos.y = current.y - pos.y;
			}
		}
	}	
}

void Enemy::freeRun() {
	ladderTransformation();

	directionHelper.x = (dPos.x > 0) - (dPos.x < 0);
	directionHelper.y = (dPos.y > 0) - (dPos.y < 0);

	checkCollisionWithOthers();
	checkCollisionsWithEnvironment();

	int nextY = int(pos.y + 0.5 + dPos.y);
	LayoutBlock nextYBlock = gameContext->getLayoutElement(current.x, nextY);

	//fall from ladder to empty, pole or trapDoor
	if (dPos.y < 0 && middle == LayoutBlock::ladder && (nextYBlock == LayoutBlock::empty || nextYBlock == LayoutBlock::pole || nextYBlock == LayoutBlock::trapDoor)) {
		initiateFallingStart();
		return;
	}

	//fall from pole
	if (dPos.y < 0 && middle == LayoutBlock::pole && (downBlock == LayoutBlock::empty || downBlock == LayoutBlock::pole || downBlock == LayoutBlock::trapDoor)) {
		if (enemyChecker(current.x, current.y - 1)) {
			dPos.y = 0;
		}
		else {
			pos.x = current.x;
			dPos.x = 0;
			initiateFallingStart();
			return;
		}
	}

	int nextX = int(pos.x + 0.5 + dPos.x);

	bool enemyUnder = false;
	bool enemyNextUnder = false;

	for (auto& enemy : gameContext->getEnemies()) {
		if (enemy.get() != this) {
			if (enemy->current.x == current.x && enemy->current.y == current.y - 1) {
				enemyUnder = true;
			}

			if (directionHelper.x != 0 && enemy->current.x == nextX && enemy->current.y == current.y - 1) {
				enemyNextUnder = true;
			}
		}
	}

	//character has to fall when an other character which was under him before, leaves the block under
	if ((middle == LayoutBlock::empty || middle == LayoutBlock::trapDoor) && (downBlock == LayoutBlock::empty || downBlock == LayoutBlock::pole || downBlock == LayoutBlock::trapDoor) && !enemyUnder) {
		initiateFallingStart();
		return;
	}
	
	LayoutBlock nextXBlock = gameContext->getLayoutElement(nextX, current.y);
	//used to check next block for starting to fall
	LayoutBlock nextUnderBlock = gameContext->getLayoutElement(nextX, current.y - 1);

	//falling from the edge of a block (brick, concrete, enemy, ladder)
	if (dPos.x != 0 && nextXBlock == LayoutBlock::empty && (nextUnderBlock == LayoutBlock::empty || nextUnderBlock == LayoutBlock::trapDoor || nextUnderBlock == LayoutBlock::pole) && !enemyNextUnder) {
		initiateFallingStart();
		return;
	}

	//if some probem arised and character is stuck in air, unstuck it!
	if (pos.x != current.x && pos.y != current.y) {
		bool correct = true;

		for (auto& enemy : gameContext->getEnemies()) {
			if (enemy.get() != this) {
				if (std::abs(enemy->pos.x - pos.x) < 1.0f && pos.y < enemy->pos.y + 1.0f) {
					correct = false;
					break;
				}
			}
		}

		if (correct) {
			initiateFallingStart();
		}
	}

	checkGoldCollect();
	checkGoldDrop();
}

void Enemy::initiateFallingStart() {
	/*if (pos.x > current.x) {
		direction = left;
	}
	else {
		direction = right;
	}*/

	state = EnemyState::startingToFall;
}

void Enemy::initiateFallingStop() {
	dPos.y = int(pos.y + dPos.y) + 1 - pos.y;
	state = EnemyState::freeRun;
}

bool Enemy::checkHole() {
	if (gameContext->getLayoutElement(current.x, current.y) == LayoutBlock::empty && gameContext->getBrickByCoordinates(current.x, current.y) && dPos.x == 0 && pos.y > current.y) {
		this->holeBrick = gameContext->getBrickByCoordinates(current.x, current.y).get();
		return true;
	}

	return false;
}

void Enemy::falling() {
	dPos.x = 0;
	dPos.y = -actualSpeed;

	if (checkHole()) {
		state = EnemyState::pitting;
		pitState = PitState::fallingToPit;
		return;
	}

	//falling into pit from falling
	int nextY = int(pos.y + dPos.y);

	LayoutBlock blockUnderFallingEnemy = gameContext->getLayoutElement(current.x, nextY);
	//Falling to brick, concrete, ladder, enemy
	if (blockUnderFallingEnemy == LayoutBlock::brick || blockUnderFallingEnemy == LayoutBlock::concrete || blockUnderFallingEnemy == LayoutBlock::ladder) {
		initiateFallingStop();
		return;
	}

	//falling onto enemy!
	for (auto& enemy : gameContext->getEnemies()) {
		if (enemy.get() != this) {
			if (current.x == enemy->current.x && enemy->pos.y < pos.y && std::abs((pos.y + dPos.y) - enemy->pos.y) < 1.0f && gameContext->getLayoutElement(enemy->current.x, enemy->current.y) != LayoutBlock::pole) {
				initiateFallingStop();
				return;
			}
		}
	}

	//falling to pole
	if (gameContext->getLayoutElement(current.x, current.y) == LayoutBlock::pole && pos.y + dPos.y <= current.y && pos.y > current.y) {
		initiateFallingStop();
		return;
	}

	checkGoldCollect();
}

void Enemy::die() {
	//Enemy::killCounter++;
	gameContext->incrementKillCounter();

	if (carriedGold) {
		carriedGold.reset();
		if (!gameContext->enemyCarriesGold() && gameContext->getUncollectedGoldSize() == 0) {
			gameContext->getAudio()->getAudioFileByID(4)->playPause();
			gameContext->generateFinishingLadders();
		}
	}

	std::vector<int> nonEmptyBlocks;
	int vertical = 15;

	//finding a row with an empty block starting from the 15th row, and going down
	while (true) {
		for (int i = 1; i < 29; i++) {
			bool checkGold = gameContext->goldChecker(i, vertical);

			if (gameContext->getLayoutElement(i, vertical) == LayoutBlock::empty && !enemyChecker(i, vertical) && !checkGold && !gameContext->getBrickByCoordinates(i, vertical)) {
				nonEmptyBlocks.push_back(i);
			}
		}

		if (nonEmptyBlocks.size() != 0) {
			break;
		}

		vertical--;
	}

	pos.y = vertical;
	short randomX = rand() % nonEmptyBlocks.size();
	pos.x = nonEmptyBlocks[randomX];
	dPos.x = 0;
	dPos.y = 0;
	state = EnemyState::dying;
	pitState = PitState::fallingToPit;
	holeBrick = nullptr;

	dieTimer = gameTime;
}

void Enemy::dying() {
	dPos.x = 0;
	dPos.y = 0;

	if ((gameTime - dieTimer) > 1) {
		state = EnemyState::falling;
	}
}

void Enemy::pitting() {
	switch (pitState) {
	case PitState::fallingToPit:
		fallingToPit();
		break;
	case PitState::moving:
		movingInPit();
		break;
	case PitState::climbing:
		climbing();
		break;
	}	
}

void Enemy::fallingToPit() {
	if (pos.x != holeBrick->getPosition().x) {

		//falling from edge left->right
		if (pos.x < holeBrick->getPosition().x) {
			direction = Direction::right;

			if (pos.x + actualSpeed > holeBrick->getPosition().x) {
				dPos.x = holeBrick->getPosition().x - pos.x;
			}
			else {
				dPos.x = actualSpeed;
			}
		}
		//falling from edge right->left
		else {
			direction = Direction::left;

			if (pos.x - actualSpeed < holeBrick->getPosition().x) {
				dPos.x = holeBrick->getPosition().x - pos.x;
			}
			else {
				dPos.x = -actualSpeed;
			}
		}

		dPos.y = -actualSpeed;
	}
	else {
		dPos.x = 0;
	}

	//do not fall into if rebuilt
	if (gameContext->getLayoutElement(holeBrick->getPosition().x, holeBrick->getPosition().y) == LayoutBlock::brick && std::abs(pos.y - holeBrick->getPosition().y) >= 0.75) {
		state = EnemyState::freeRun;
		pitState = PitState::fallingToPit;
		pos.y = holeBrick->getPosition().y + 1;

		if (gameContext->getLayoutElement(holeBrick->getPosition().y, holeBrick->getPosition().y + 1) == LayoutBlock::brick) {
			die();
		}
		else {
			state = EnemyState::freeRun;
		}

		holeBrick = nullptr;
		dPos.y = 0;
		return;
	}

	dPos.y = -actualSpeed;	

	Vector2DInt brickPosition = holeBrick->getPosition();

	if (int(pos.y + dPos.y) < brickPosition.y) {
		dPos.y = brickPosition.y - pos.y;
		pitState = PitState::moving;
		holeTimer = gameTime;

		if (carriedGold) {
			carriedGold->setPos({ brickPosition.x, brickPosition.y + 1 });
			gameContext->addGoldToUncollectedList(carriedGold);
			carriedGold = nullptr;
		}
	}
}

void Enemy::movingInPit() {
	//being idle in the hole for a moment
	if (gameTime - holeTimer < holeIdle) {
		dPos.x = 0;
		dPos.y = 0;
	}

	//moving horizontally in hole after being idle
	else if (holeIdle < gameTime - holeTimer && gameTime - holeTimer < holeIdle + holeHorizontalTime) {
		//"harmonic motion" in pit
		pos.x = holeBrick->getPosition().x + 0.25f * sin(2 * 3.14159265359f / holeHorizontalTime * (gameTime - (holeIdle + holeTimer)));
		dPos.x = 0;
		dPos.y = 0;
	}

	//going up to the half of the hole block!
	else if (gameTime - holeTimer > holeIdle + holeHorizontalTime) {
		pos.x = holeBrick->getPosition().x;

		//if higher than 3/4 of the block try climbing
		if (pos.y + actualSpeed > holeBrick->getPosition().y + 0.75f) {
			pos.y = holeBrick->getPosition().y + 0.75f;
			dPos.y = 0;

			pitState = PitState::climbing;

			if (dPos.x > 0) {
				direction = Direction::right;
			}
			if (dPos.x < 0) {
				direction = Direction::left;
			}
		}
		else {
			pos.y += actualSpeed;
			dPos.y = 0;

			for (auto& enemy : gameContext->getEnemies()) {
				if (enemy.get() != this) {
					if (std::abs(enemy->pos.x - holeBrick->getPosition().x) < 0.5f && enemy->pos.y < holeBrick->getPosition().y + 2) {
						pitState = PitState::fallingToPit;
						break;
					}
				}
			}
		}

		dPos.x = 0;
	}
	return;
}

void Enemy::climbing() {
	//if horizontally stopped, fall back!
	if (dPos.x == 0) {
		//this needed when coming out from hole and next it there is a pole or ladder
		//needed because different pathfinding implemantation
		if (dPos.y != 0) {
			if (direction == Direction::left) {
				dPos.x = -actualSpeed;
			}
			if (direction == Direction::right) {
				dPos.x = +actualSpeed;
			}
		}
		else {
			//std::cout << "\n falling back dpx0";
			pitState = PitState::fallingToPit;
			return;
		}
	}
	
	Vector2DInt brickPosition = holeBrick->getPosition();

	//if direction changed, fall back!
	if ((dPos.x > 0 && direction == Direction::left) || (dPos.x < 0 && direction == Direction::right)) {
		pitState = PitState::fallingToPit;
		return;
	}
	//continue climbing
	else {
		dPos.y = actualSpeed;

		//do not go higher than block
		if (pos.y + dPos.y > brickPosition.y + 1) {
			dPos.y = brickPosition.y + 1 - pos.y;
		}

		//fall back if the above brick is rebuilt
		if (gameContext->getLayoutElement(brickPosition.x, brickPosition.y + 1) == LayoutBlock::brick) {
			pitState = PitState::fallingToPit;
			return;
		}

		//fallback if exit block is rebuilt on left
		if (dPos.x < 0) {
			if (gameContext->getLayoutElement(brickPosition.x - 1, brickPosition.y + 1) == LayoutBlock::brick) {
				pitState = PitState::fallingToPit;
				return;
			}
		}

		//fallback if exit block is rebuilt on right
		else if (dPos.x > 0) {
			if (gameContext->getLayoutElement(brickPosition.x + 1, brickPosition.y + 1) == LayoutBlock::brick) {
				pitState = PitState::fallingToPit;
				return;
			}
		}

		//fall back if 'exit block' is occupied by an other enemy (exit block == the block where enemy will got after exiting -  x = hole.x+-1, y = hole.y+1
		for (auto& enemy : gameContext->getEnemies()) {
			if (enemy.get() != this)
			{
				if (dPos.x < 0) {
					//if (holepos.x - 1.55f < enemies[i]->pos.x && enemies[i]->pos.x < holepos.x && int(enemies[i]->pos.y + 0.5) == holepos.y + 1) {
					if (pos.x - 1 < enemy->pos.x && enemy->pos.x < pos.x && enemy->current.y == brickPosition.y + 1) {
						//std::cout << "\n falling back from left to right, because enemy is above!";
						pitState = PitState::fallingToPit;
						return;
					}
				}
				else if (dPos.x > 0) {
					//if (holepos.x < enemies[i]->pos.x && enemies[i]->pos.x < holepos.x + 1.55 && int(enemies[i]->pos.y + 0.5) == holepos.y + 1) {
					if (pos.x < enemy->pos.x && enemy->pos.x < pos.x + 1 && enemy->current.y == brickPosition.y + 1) {
						//std::cout << "\n falling back from right to left, because enemy is above!";
						pitState = PitState::fallingToPit;
						return;
					}
				}
			}
		}

		//out!!
		if (std::abs((pos.x + dPos.x) - brickPosition.x) > 0.55f) {
			//std::cout << "\n out from pit!";
				
			if (dPos.x < 0) {
				pos.x = brickPosition.x - 0.55f;
				dPos.x = 0;
			}
			else if (dPos.x > 0) {
				pos.x = brickPosition.x + 0.55f;
				dPos.x = 0;
			}

			dPos.y = 0;

			state = EnemyState::freeRun;
			pitState = PitState::fallingToPit;
			holeBrick = nullptr;
		}
	}
}

//-----------------------------------------------------------
//------------------------ANIMATE----------------------------
void Enemy::animateFreeRun() {
	//going animation
	if (pos.x - prevPos.x != 0  && (middle == LayoutBlock::empty || middle == LayoutBlock::ladder || middle == LayoutBlock::trapDoor)) {
		animateGoing();
	}
	//on ladder, or coming out from pit
	else if (pos.y - prevPos.y != 0 && (middle == LayoutBlock::ladder || downBlock == LayoutBlock::ladder)) {
		animateOnLadder();
	}
	//on pole
	else if (middle == LayoutBlock::pole && (pos.x - prevPos.x != 0 || pos.y - prevPos.y != 0) && current.y == pos.y) {
		animateOnPole();
	}
}

void Enemy::animateDying() {
	*texturePointer = textureMap.death + int(4 * (gameTime - dieTimer)) % 4;
}

void Enemy::animateDigging() {
	
}

void Enemy::animateFalling() {
	*texturePointer = textureMap.falling + animationTimeFactor;
}

void Enemy::animateGoing() {
	*texturePointer = textureMap.going + animationTimeFactor;
}

void Enemy::animateOnLadder() {
	*texturePointer = textureMap.ladder + animationTimeFactor;
}

void Enemy::animateOnPole() {
	*texturePointer = textureMap.pole + animationTimeFactor;
}

void Enemy::animatePitting() {
	switch (pitState) {
	case PitState::fallingToPit:
		animateFalling();
		break;

	case PitState::moving:
		if (pos.y - prevPos.y != 0 && gameTime - holeTimer > 1) {
			animateOnLadder();
		}
		else {
			*texturePointer = textureMap.falling;
		}
			
		break;

	case PitState::climbing:
		if (pos.y - prevPos.y != 0) {
			animateOnLadder();
		}
		else {
			animateGoing();
		}

		break;
	}
}

void Enemy::checkGoldCollect() {
	//if not carryiing and there is one, carry it
	if (!carriedGold) {
		if ((carriedGold = gameContext->goldCollectChecker(pos.x, pos.y))) {
			carriedGold->setReleaseCounter(rand() % 26 + 14);
			carriedGold->setPos(Vector2DInt{-1, -1});
		}		
	}
}

//if already carrying, check to drop
void Enemy::checkGoldDrop() {
	if (carriedGold) {
		int prevX = int(prevPos.x + 0.5);
		int prevY = int(prevPos.y + 0.5);

		if (int(pos.x + dPos.x + 0.5) != prevX || int(pos.y + dPos.y + 0.5) != prevY) {
			if (carriedGold->shouldBeReleased()) {
				bool checkGold = gameContext->goldChecker(prevX, prevY);
				LayoutBlock prevBlock = gameContext->getLayoutElement(prevX, prevY);
				LayoutBlock prevBlockUnder = gameContext->getLayoutElement(prevX, prevY - 1);

				if (prevBlock == LayoutBlock::empty && !gameContext->getBrickByCoordinates(prevX, prevY) && !checkGold && (prevBlockUnder == LayoutBlock::brick || prevBlockUnder == LayoutBlock::concrete || prevBlockUnder == LayoutBlock::ladder)) {
					carriedGold->setPos({ prevX, prevY });

					gameContext->addGoldToUncollectedList(carriedGold);
					carriedGold = nullptr;
				}
			}
		}
	}
}