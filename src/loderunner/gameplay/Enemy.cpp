#include "Enemy.h"

#include "iocontext/audio/AudioFile.h"
#include "states/gamestates/Play.h"

#include "GameElements.h"

#include <cmath>

Enemy::Enemy(float x, float y, bool player) {
	this->pos = { x, y };

	if (player) {
		enemyTextureMap = { 28, 52, 48, 36, 40, 26 };
	}
}

void Enemy::determineNearbyObjects(std::shared_ptr<GameElements>& gameElements, Vector2D cPos) {
	current.x = int(pos.x + 0.5);
	current.y = int(pos.y + 0.5);

	middle = gameElements->layout[current.x][current.y];
	downBlock = gameElements->layout[current.x][current.y - 1];

	direction = dPos.x > 0 ? Direction::right : (dPos.x < 0 ? Direction::left : direction);

	propertyPointer[0] = direction == Direction::left;
	propertyPointer[1] = goldCounter.has_value();
}

Vector2DInt Enemy::findPath(std::shared_ptr<GameElements>& gameElements) {
	//reminder for implementing the moving from the other document
	//base is untouched - ie. it is the same as the text file
	//golds are in .base 
	//enemies are in .act and not in base!
	//if a hole is created then the value of brick in act is emptied, but in the base it is unchanged!

	int x = current.x;
	int y = current.y;

	Vector2DInt runnerPos = gameElements->getPlayerPosition();

	if (y == runnerPos.y && gameElements->getPlayerState() != EnemyState::falling) {
		while (x != runnerPos.x) {
			LayoutBlock checkable = gameElements->layout[x][y];
			LayoutBlock belowCheckable = gameElements->layout[x][y - 1];

			if (checkable == LayoutBlock::ladder || checkable == LayoutBlock::pole || belowCheckable == LayoutBlock::brick || gameElements->isBrick(x, y - 1) ||
				belowCheckable == LayoutBlock::concrete || belowCheckable == LayoutBlock::ladder || belowCheckable == LayoutBlock::pole ||
				gameElements->isEnemy(x, y - 1) || gameElements->isGold(x, y - 1)) {

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
				return {1, 0};
			}
			else if (pos.x > runnerPos.x) {
				return {-1, 0};
			}
			else {
				//this conditions are not necessary due to different implementations
				//if (player->dPos.x < 0) {
					//return {-1.0f, 1.0f};
				//}
				//else {
					//return {1.0f, 1.0f};
				//}
			}
			//return;
		}
	}

	return scanFloor(gameElements);
}

Vector2DInt Enemy::scanFloor(std::shared_ptr<GameElements>& gameElements) {
	int startX = current.x;
	//int startY = current.y;

	int x = current.x;
	int y = current.y;

	bestPath = 0;
	bestRating = 255;

	while (x > 0) {                                    //get left end first
		LayoutBlock checkable = gameElements->layout[x - 1][y];
		LayoutBlock belowCheckable = gameElements->layout[x - 1][y - 1];

		if (checkable == LayoutBlock::brick || checkable == LayoutBlock::concrete) {
			break;
		}
		if (checkable == LayoutBlock::ladder || checkable == LayoutBlock::pole || belowCheckable == LayoutBlock::brick || gameElements->isBrick(x - 1, y - 1) || belowCheckable == LayoutBlock::concrete || belowCheckable == LayoutBlock::ladder) {
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
		LayoutBlock checkable = gameElements->layout[x + 1][y];;
		LayoutBlock belowCheckable = gameElements->layout[x + 1][y - 1];

		if (checkable == LayoutBlock::brick || checkable == LayoutBlock::concrete) {
			break;
		}
		if (checkable == LayoutBlock::ladder || checkable == LayoutBlock::pole || belowCheckable == LayoutBlock::brick || gameElements->isBrick(x + 1, y - 1) || belowCheckable == LayoutBlock::concrete || belowCheckable == LayoutBlock::ladder) {
			++x;
		}
		else {                                         // go on right anyway
			++x;
			break;
		}
	}

	int rightEnd = x;

	x = startX;
	LayoutBlock checkable = gameElements->layout[x][y];
	LayoutBlock belowCheckable = gameElements->layout[x][y - 1];

	//the possible directions (+0: stay), this numbers are the second arguments of the scandown and scanup functions (for the best path)
	//	1: left
	//	2: right
	//  3: up
	//  4: down
	if (belowCheckable != LayoutBlock::brick && !gameElements->isBrick(x, y - 1) && belowCheckable != LayoutBlock::concrete) {
		scanDown(x, 4, gameElements);
	}
	if (checkable == LayoutBlock::ladder) {
		scanUp(x, 3, gameElements);
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

		checkable = gameElements->layout[x][y];
		belowCheckable = gameElements->layout[x][y - 1];

		if (belowCheckable != LayoutBlock::brick && !gameElements->isBrick(x, y - 1) && belowCheckable != LayoutBlock::concrete) {
			scanDown(x, curPath, gameElements);
		}

		if (checkable == LayoutBlock::ladder) {
			scanUp(x, curPath, gameElements);
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
		return {-1, 0};
		break;

	case 2:
		return {1, 0};
		break;
	case 3:
		return {0, 1};
		break;

	case 4:
		return {0, -1};
		break;

	default:
		break;
	}

	 return {0, 0};
}

void Enemy::scanDown(int x, int curPath, std::shared_ptr<GameElements>& gameElements) {
	int y = current.y;
	int runnerY = gameElements->getPlayerPosition().y;

	LayoutBlock belowCheckable = gameElements->layout[x][y - 1];

	while (y > 0 && (belowCheckable = gameElements->layout[x][y - 1]) != LayoutBlock::brick && !gameElements->isBrick(x, y - 1) && belowCheckable != LayoutBlock::concrete) {
		//here is a problem, with champ 19?
		if (gameElements->layout[x][y] != LayoutBlock::empty || gameElements->isGold(x, y) || gameElements->isBrick(x, y)) {
			if (x > 1) {
				LayoutBlock leftSideBelow = gameElements->layout[x - 1][y - 1];
				LayoutBlock leftSide = gameElements->layout[x - 1][y];

				if (leftSideBelow == LayoutBlock::brick || gameElements->isBrick(x - 1, y - 1) || leftSideBelow == LayoutBlock::concrete || leftSideBelow == LayoutBlock::ladder || leftSide == LayoutBlock::pole) {
					if (y <= runnerY) {
						break;
					}
				}
			}

			if (x < 28) {
				LayoutBlock rightSideBelow = gameElements->layout[x + 1][y - 1];
				LayoutBlock rightSide = gameElements->layout[x + 1][y];

				if (rightSideBelow == LayoutBlock::brick || gameElements->isBrick(x + 1, y - 1) || rightSideBelow == LayoutBlock::concrete || rightSideBelow == LayoutBlock::ladder || rightSide == LayoutBlock::pole) {
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

void Enemy::scanUp(int x, int curPath, std::shared_ptr<GameElements>& gameElements) {
	int y = current.y;
	int runnerY = gameElements->getPlayerPosition().y;

	while (y < 18 && gameElements->layout[x][y] == LayoutBlock::ladder) {
		++y;

		LayoutBlock leftSideBelow = gameElements->layout[x - 1][y - 1];
		LayoutBlock leftSide = gameElements->layout[x - 1][y];

		LayoutBlock rightSideBelow = gameElements->layout[x + 1][y - 1];
		LayoutBlock rightSide = gameElements->layout[x + 1][y];

		if (x > 1) {
			if (leftSideBelow == LayoutBlock::brick || gameElements->isBrick(x - 1, y - 1) || leftSideBelow == LayoutBlock::concrete || leftSideBelow == LayoutBlock::ladder || leftSide == LayoutBlock::pole) {
				if (y >= runnerY) {
					break;
				}
			}
		}

		if (x < 28) {
			if (rightSideBelow == LayoutBlock::brick || gameElements->isBrick(x + 1, y - 1) || rightSideBelow == LayoutBlock::concrete || rightSideBelow == LayoutBlock::ladder || rightSide == LayoutBlock::pole) {
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

//To properly use ladder characters need to be adjusted into center!
Vector2D Enemy::ladderTransformation(Vector2D d, std::shared_ptr<GameElements>& gameElements) {
	//you can move on a ladder 1.5 unit vertically, middle block only covers the bottom 2/3 of the ladder, tophalfladder covers the upper 2/3 of ladder
	LayoutBlock topHalfOfLadder = gameElements->layout[current.x][int(pos.y)];
	if (topHalfOfLadder != LayoutBlock::ladder && middle != LayoutBlock::ladder && d.y > 0 && pos.y + d.y > current.y) {
		d.y = current.y - pos.y;
	}

	if (middle != LayoutBlock::ladder && downBlock != LayoutBlock::ladder && middle != LayoutBlock::pole && d.y < 0) {
		d.y = 0;
	}

	//Handle when multiple buttons pushed (only used by runner)
	if (d.x != 0 && d.y != 0) {
		if (d.y > 0) {
			if (middle == LayoutBlock::ladder) {
				d.x = 0;
			}
			else if (downBlock == LayoutBlock::ladder) {
				d.y = 0;
			}
		}
		else {
			if (middle == LayoutBlock::ladder) {
				if (downBlock == LayoutBlock::ladder || downBlock == LayoutBlock::pole || downBlock == LayoutBlock::empty || downBlock == LayoutBlock::trapDoor) {
					d.x = 0;
				}
				else {//if (downBlock != ladder) {
					d.y = 0;
				}
			}
			else if (downBlock == LayoutBlock::ladder) {
				d.x = 0;
			}
		}			
	}

	//adjust character to middle (used by everyone)
	if (middle == LayoutBlock::ladder || downBlock == LayoutBlock::ladder) {
		if (d.x != 0 && pos.y != current.y) {
			float dy = std::abs(d.x);
			d.x = 0;			
			if (pos.y > current.y) {
				d.y = pos.y - dy < current.y ? current.y - pos.y : -dy;
			}
			else if (pos.y < current.y) {
				d.y = pos.y + dy > current.y ? current.y - pos.y : dy;
			}
		}
		else if (d.y != 0 && pos.x != current.x) {
			float dx = std::abs(d.y);
			d.y = 0;
			if (pos.x > current.x) {
				d.x = pos.x - dx < current.x ? current.x - pos.x : -dx;
			}
			else if (pos.x < current.x) {
				d.x = pos.x + dx > current.x ? current.x - pos.x : dx;
			}
		}
	}

	return d;
}

Vector2D Enemy::checkCollisionsWithEnvironment(Vector2D d, std::shared_ptr<GameElements>& gameElements) {
	if (d.x != 0) {
		short directionX = (d.x > 0) - (d.x < 0);
		int nextX = current.x + directionX;
		LayoutBlock nextXBlock = gameElements->layout[nextX][current.y];

		if ((nextXBlock == LayoutBlock::brick || nextXBlock == LayoutBlock::concrete || nextXBlock == LayoutBlock::trapDoor) && std::abs(nextX - pos.x - d.x) < 1.0f) {
			d.x = current.x - pos.x;
		}
	}	

	if (d.y != 0) {
		if (d.y > 0 && middle != LayoutBlock::ladder && pos.y + d.y > current.y) {
			d.y = current.y - pos.y;
		}
		
		//TODO
		if (d.y + pos.y > 17) {
			d.y = 17 - pos.y;
		}

		short directionY = (d.y > 0) - (d.y < 0);		
		if ((pos.y + d.y - current.y) * directionY <= 0) {
			return d;
		}

		int nextY = current.y + directionY;
		LayoutBlock nextYBlock = gameElements->layout[current.x][nextY];

		if (std::abs(nextY - pos.y - d.y) < 1.0f) {
			if (nextYBlock == LayoutBlock::brick || nextYBlock == LayoutBlock::concrete || (nextYBlock == LayoutBlock::trapDoor && d.y > 0)) {
				d.y = current.y - pos.y;
			}
		}
	}	

	return d;
}

Vector2D Enemy::handleDropping(Vector2D d, std::shared_ptr<GameElements>& gameElements) {
	int nextY = int(pos.y + 0.5 + d.y);
	LayoutBlock nextYBlock = gameElements->layout[current.x][nextY];

	//fall from ladder to empty, pole or trapDoor
	if (d.y < 0 && middle == LayoutBlock::ladder && (nextYBlock == LayoutBlock::empty || nextYBlock == LayoutBlock::pole || nextYBlock == LayoutBlock::trapDoor)) {
		state = EnemyState::falling;
		return d;
	}

	//fall from pole
	if (d.y < 0 && middle == LayoutBlock::pole 
		&& (downBlock == LayoutBlock::empty || downBlock == LayoutBlock::pole || downBlock == LayoutBlock::trapDoor)) {
		d.x = current.x - pos.x;
		state = EnemyState::falling;
		return d;
	}	

	//character has to fall when an other character which was under him before, leaves the block under
	if ((middle == LayoutBlock::empty || middle == LayoutBlock::trapDoor) && (downBlock == LayoutBlock::empty || downBlock == LayoutBlock::pole || downBlock == LayoutBlock::trapDoor) &&
		!gameElements->isEnemyUnder(this)) {
		state = EnemyState::falling;
		return d;
	}
	
	int nextX = int(pos.x + 0.5 + d.x);
	if (nextX != current.x) {		
		LayoutBlock nextXBlock = gameElements->layout[nextX][current.y];
		LayoutBlock nextUnderBlock = gameElements->layout[nextX][current.y - 1];

		//falling from the edge of a block (brick, concrete, enemy, ladder)
		if (nextXBlock == LayoutBlock::empty && (nextUnderBlock == LayoutBlock::empty || nextUnderBlock == LayoutBlock::trapDoor || nextUnderBlock == LayoutBlock::pole) &&
			!gameElements->isEnemyUnderNext(this, pos.x + d.x)) {
			state = EnemyState::falling;
			return d;
		}
	}

	return d;
}

Vector2D Enemy::falling(float d, std::shared_ptr<GameElements>& gameElements) {
	int nextY = int(pos.y - d);
	
	if (pos.x < current.x) {
		direction = Direction::right;
		float dx = pos.x + d > current.x ? current.x - pos.x : d;

		return { dx, -d };
	}
	//falling from edge right->left
	else if (pos.x > current.x) {
		direction = Direction::left;
		float dx = pos.x - d < current.x ? current.x - pos.x : -d;

		return { dx, -d };
	}

	Vector2D stoppedPath = { 0.0f, int(pos.y - d) + 1 - pos.y };
	
	LayoutBlock blockUnderFallingEnemy = gameElements->layout[current.x][nextY];
	//Falling to brick, concrete, ladder, enemy
	if (blockUnderFallingEnemy == LayoutBlock::brick || blockUnderFallingEnemy == LayoutBlock::concrete || blockUnderFallingEnemy == LayoutBlock::ladder) {
		state = EnemyState::freeRun;
		return stoppedPath;
	}

	//falling to pole
	if (gameElements->layout[current.x][current.y] == LayoutBlock::pole && pos.y - d <= current.y && pos.y > current.y) {
		state = EnemyState::freeRun;
		return stoppedPath;
	}

	if (gameElements->isEnemyUnderFalling(this, pos.y - d)) {
		state = EnemyState::freeRun;
		return stoppedPath;
	}

	return {0, -d};
}

Vector2D Enemy::pitting(Vector2D d, float speed, float gameTime, std::shared_ptr<GameElements>& gameElements) {
	//being idle in the hole for a moment
	float ellapsedTime = gameTime - timer;

	int brickX = hole.value().x;
	int brickY = hole.value().y;	

	if (ellapsedTime < holeIdle) {
		return { current.x - pos.x, 0.0f };
	}

	//moving horizontally in hole after being idle, "harmonic motion" in pit
	if (holeIdle < ellapsedTime && ellapsedTime < holeIdle + holeHorizontalTime) {
		direction = pos.x - current.x > 0 ? Direction::left : Direction::right;
		return { current.x + 0.25f * static_cast<float>(sin(2 * 3.14159265359f / holeHorizontalTime * (ellapsedTime - holeIdle))) - pos.x, 0.0f };
	}

	//going up to the half of the hole block!
	if (pos.y + speed > brickY + 1) {
		d.y = brickY + 1 - pos.y;
	}
	else {
		d.y = speed;
	}

	if (pos.y < brickY + 0.75f) {
		d.x = current.x - pos.x;
		
		if (d.x > 0) {
			direction = Direction::right;
		}
		else if (d.x < 0) {
			direction = Direction::left;
		}
		else {

		}

		if (gameElements->isEnemy(brickX, brickY + 1, 1.0f, 0.1f)) {
			hole.reset();
			state = EnemyState::falling;
		}
	}
	else {
		//if horizontally stopped, fall back!
		if (d.x == 0) {
			//this needed when coming out from hole and next it there is a pole or ladder
			//needed because different pathfinding implemantation
			if (d.y != 0) {
				if (direction == Direction::left) {
					d.x = -speed;
				}
				else if (direction == Direction::right) {
					d.x = +speed;
				}
			}
			else {
				hole.reset();
				state = EnemyState::falling;
				return d;
			}
		}

		//if direction changed, fall back!
		if (pos.x != current.x && ((d.x > 0 && direction == Direction::left) || (d.x < 0 && direction == Direction::right))) {
			hole.reset();
			state = EnemyState::falling;
			return d;
		}
		//continue climbing
		else {
			//fall back if the above brick is rebuilt
			if (gameElements->layout[brickX][brickY + 1] == LayoutBlock::brick) {
				hole.reset();
				state = EnemyState::falling;
				return d;
			}

			//fallback if exit block is rebuilt on 
			if (d.x != 0) {
				int direction = (d.x > 0) - (d.x < 0);
				int nextX = brickX + direction;

				if (gameElements->layout[nextX][brickY + 1] == LayoutBlock::brick) {
					hole.reset();
					state = EnemyState::falling;
					return d;
				}

				//fall back if 'exit block' is occupied by an other enemy (exit block == the block where enemy will got after exiting -  x = hole.x+-1, y = hole.y+1				
				if (gameElements->isEnemyNearHole(this, brickX, brickY)) {
					hole.reset();
					state = EnemyState::falling;
					return d;
				}
			}

			int outX = int(pos.x + d.x + 0.5f);
			if (brickX != outX) {
				d.x = brickX + (outX - brickX) * 0.51f - pos.x;
				hole.reset();
				state = EnemyState::freeRun;
			}	
		}
	}

	return d;
}
