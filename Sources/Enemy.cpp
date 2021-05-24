#include "Enemy.h"
#include "Gold.h"
#include "GameTime.h"
#include "GameStates/Play.h"
#include "IOHandler.h"

std::vector<std::unique_ptr<Enemy>> Enemy::enemies;
std::unique_ptr<Enemy> Enemy::player;
int Enemy::animationFactor = 20;
LayoutBlock** Enemy::layout;
Play* Enemy::play;
std::unique_ptr<Brick>** Enemy::brickO;
std::unique_ptr<Trapdoor>** Enemy::trapdoors;
unsigned int Enemy::killCounter = 0;;
float Enemy::playerSpeed = 0.9f;
float Enemy::enemySpeed = 0.415f;

void Enemy::DrawPaused() {
	DrawEnemy(player->Pos.x, player->Pos.y, player->TextureRef, player->direction, false);

	for (auto& enemy : enemies) {
		DrawEnemy(enemy->Pos.x, enemy->Pos.y, enemy->TextureRef, enemy->direction, enemy->carriedGold != nullptr);
	}
}

void Enemy::DrawPlayerDeath() {
	//player->Dying();

	for (auto& enemy : enemies) {
		DrawEnemy(enemy->Pos.x, enemy->Pos.y, enemy->TextureRef, enemy->direction, enemy->carriedGold != nullptr);
	}
}

unsigned int Enemy::GetKillCounter() {
	return Enemy::killCounter;
}

void Enemy::SetPlayerSpeed(float playerSpeed) {
	Enemy::playerSpeed = playerSpeed;
}

void Enemy::SetEnemySpeed(float enemySpeed) {
	Enemy::enemySpeed = enemySpeed;
}

float Enemy::gameTime = 0;

int Enemy::bestPath;
float Enemy::bestRating;

void Enemy::HandlePlayerDying() {
	player->Dying();
}

bool Enemy::HasGold() {
	for (auto& enemy : enemies) {
		if (enemy->carriedGold) {
			return true;
		}
	}

	return false;
}

void Enemy::setLayoutPointers(LayoutBlock** layout, std::unique_ptr<Brick>** brickO, std::unique_ptr<Trapdoor>** trapdoors, Play* play) {
	Enemy::layout = layout;
	Enemy::brickO = brickO;
	Enemy::play = play;
	Enemy::trapdoors = trapdoors;
}

void Enemy::AddEnemy(Vector2DInt position) {
	Enemy* enemy = new Enemy();
	enemy->Pos = { (float) position.x, (float) position.y };
	enemy->prevPos = enemy->Pos;

	std::unique_ptr<Enemy> uniquePointer(enemy);

	enemies.push_back(std::move(uniquePointer));
}

void Enemy::NotifyPlayerAboutDigEnd() {
	player->ReleaseFromDigging();
}

void Enemy::clearEnemyVector() {
	enemies.clear();
	killCounter = 0;
}

Enemy::Enemy() {
	dPos.x = 0;
	dPos.y = 0;

	dPrevPos.x = 0;
	dPrevPos.y = 0;

	direction = right;
	pitState = fallingToPit;
	state = freeRun;

	TextureRef = textureMap.going;

	carriedGold = nullptr;
	charSpeed = enemySpeed;
}

bool Enemy::CheckDigPrevention(int x, int y) {
	for (auto& enemy : enemies) {
		if (abs(enemy->Pos.x - x) <= 0.75f && y + 1 <= enemy->Pos.y && enemy->Pos.y < y + 1.5f) {
			return true;
		}
	}

	return false;
}

void Enemy::CheckDeaths(int x, int y) {
	player->CheckDeath(x, y);
	
	for (auto& enemy : enemies) {
		enemy->CheckDeath(x, y);
	}
}

void Enemy::CheckDeath(int x, int y) {
	if (abs(Pos.x - x) < 0.5f && y - 0.5f <= Pos.y && Pos.y < y + 0.75f) {
		Die();
	}
}

bool Enemy::EnemyChecker(float x, float y) {
	for (auto& enemy : enemies) {
		if (enemy.get() != this) {
			//if (abs(Pos.x - enemy->Pos.x) < 0.5 && abs(Pos.x - enemy->Pos.y) < 0.5) {
			if (abs(x - enemy->Pos.x) < 0.5f && abs(y - enemy->Pos.y) < 0.5f) {
				return true;
			}
		}
	}

	return false;
}

bool Enemy::EnemyCheckerGlobal(float x, float y) {
	for (auto& enemy : enemies) {
		if (abs(x - enemy->Pos.x) < 0.5 && abs(y - enemy->Pos.y) < 0.5) {
			return true;
		}
	}

	return false;
}

void Enemy::handleCharacters() {
#ifndef RELEASE_VERSION
	enemies[0]->dPos.x = debugPos[0].x;
	enemies[0]->dPos.y = debugPos[0].y;
	enemies[1]->dPos.x = debugPos[1].x;
	enemies[1]->dPos.y = debugPos[1].y;
#endif // !RELEASE_VERSION

	gameTime = GameTime::getGameTime();

	player->handle();

	for (auto& enemy : enemies) {
		enemy->handle();
	}

#ifndef RELEASE_VERSION
	debugPos[0].x = 0;
	debugPos[0].y = 0;
	debugPos[1].x = 0;
	debugPos[1].y = 0;
#endif

}

void Enemy::handle() {
	DetermineNearbyObjects();
	FindPath();	
	Move();

	Pos.x += dPos.x;
	Pos.y += dPos.y;

	if (prevPos.x != Pos.x && state != digging) {
		if (Pos.x - prevPos.x > 0) {
			direction = right;
		}
		else if (Pos.x - prevPos.x < 0) {
			direction = left;
		}
	}

	dPos.x = 0;
	dPos.y = 0;

	Animate();

	DrawEnemy(Pos.x, Pos.y, TextureRef, direction, carriedGold != nullptr);

	dPrevPos.x = Pos.x - prevPos.x;
	dPrevPos.y = Pos.y - prevPos.y;

	prevPos.x = Pos.x;
	prevPos.y = Pos.y;
}

void Enemy::DetermineNearbyObjects() {
	curX = int(Pos.x + 0.5);
	curY = int(Pos.y + 0.5);

	middle = layout[curX][curY];
	downBlock = layout[curX][curY - 1];
}

void Enemy::DetermineDirection() {

}

//-----------------------------------------------------------
//----------------------PATH FINDING-------------------------

void Enemy::FindPath() {
	//reminder for implementing the moving from the other document
	//base is untouched - ie. it is the same as the text file
	//golds are in .base 
	//enemies are in .act and not in base!
	//if a hole is created then the value of brick in act is emptied, but in the base it is unchanged!
	actualSpeed = charSpeed * GameTime::getSpeed();

	int x = curX;
	int y = curY;

	int runnerX = player->curX;
	int runnerY = player->curY;
	
	//while
	if (y == runnerY && player->state != falling) {
		while (x != runnerX) {
			//std::cout << "\n x != runnerx";
			LayoutBlock checkable = layout[x][y];
			LayoutBlock belowCheckable = layout[x][y - 1];

			bool checkGold = Gold::GoldChecker(x, y - 1);

			if (checkable == ladder || checkable == pole || /*belowCheckable == brick ||*/ brickO[x][y - 1] || belowCheckable == concrete 
				|| belowCheckable == ladder	|| EnemyChecker(x, y - 1) || belowCheckable == pole || checkGold) {

				//guard left to runner
				if (x < runnerX) {
					++x;
				}					

				//guard right to runner
				else if (x > runnerX) {
					--x;
				}					
			}
			else {
				break;						 // exit loop with closest x if no path to runner
			}				
		}

		//scan for a path ignoring walls is a success
		if (x == runnerX) {
			if (Pos.x < runnerX) {
				dPos.x = actualSpeed;
				dPos.y = 0;
			}
			else if (Pos.x > runnerX) {
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

	ScanFloor();
}

void Enemy::ScanFloor() {
	int startX = curX;
	//int startY = curY;

	int x = curX;
	int y = curY;

	bestPath = 0;
	bestRating = 255;

	while (x > 0) {                                    //get left end first
		LayoutBlock checkable = layout[x - 1][y];
		LayoutBlock belowCheckable = layout[x - 1][y - 1];

		if (checkable == brick || checkable == concrete) {
			break;
		}

		if (checkable == ladder || checkable == pole /*|| belowCheckable == brick*/	|| brickO[x - 1][y - 1] || belowCheckable == concrete || belowCheckable == ladder) {
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
		LayoutBlock checkable = layout[x + 1][y];;
		LayoutBlock belowCheckable = layout[x + 1][y - 1];

		if (checkable == brick || checkable == concrete) {
			break;
		}			
		if (checkable == ladder || checkable == pole /*|| belowCheckable == brick */ || brickO[x + 1][y - 1] || belowCheckable == concrete || belowCheckable == ladder) {
			++x;
		}			
		else {                                         // go on right anyway
			++x;
			break;
		}
	}

	int rightEnd = x;

	x = startX;
	LayoutBlock checkable = layout[x][y];
	LayoutBlock belowCheckable = layout[x][y - 1];

	//the possible directions (+0: stay), this numbers are the second arguments of the scandown and scanup functions (for the best path)
	//	1: left
	//	2: right
	//  3: up
	//  4: down

	if (/*belowCheckable != brick &&*/ !brickO[x][y - 1] && belowCheckable != concrete) {
		ScanDown(x, 4);
	}		
	if (checkable == ladder) {
		ScanUp(x, 3);
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

		checkable = layout[x][y];
		belowCheckable = layout[x][y - 1];

		if (/*belowCheckable != brick &&*/ !brickO[x][y - 1] && belowCheckable != concrete) {
			ScanDown(x, curPath);
		}

		if (checkable == ladder) {
			ScanUp(x, curPath);
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

void Enemy::ScanDown(int x, int curPath) {
	int y = curY;
	int runnerY = player->curY;

	LayoutBlock belowCheckable = layout[x][y - 1];

	while (y > 0 && (belowCheckable = layout[x][y - 1]) != brick && !brickO[x][y - 1] && belowCheckable != concrete) {
		bool checkGold = Gold::GoldChecker(x, y);

		//here is a problem, with champ 19?
		if (layout[x][y] != empty || checkGold || brickO[x][y]) {
			if (x > 1) {
				LayoutBlock leftSideBelow = layout[x - 1][y - 1];
				LayoutBlock leftSide = layout[x - 1][y];

				if (leftSideBelow == brick || brickO[x - 1][y - 1] || leftSideBelow == concrete || leftSideBelow == ladder || leftSide == pole) {
					if (y <= runnerY) {
						break;
					}
				}
			}

			if (x < 28) {
				LayoutBlock rightSideBelow = layout[x + 1][y - 1];
				LayoutBlock rightSide = layout[x + 1][y];

				if (rightSideBelow == brick || brickO[x + 1][y - 1] || rightSideBelow == concrete || rightSideBelow == ladder || rightSide == pole) {
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
		curRating = abs(curX - x);				// update best rating and direct.
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

void Enemy::ScanUp(int x, int curPath) {
	int y = curY;
	int runnerY = player->curY;

	while (y < 18 && layout[x][y] == ladder) {
		++y;

		LayoutBlock leftSideBelow = layout[x - 1][y - 1];
		LayoutBlock leftSide = layout[x - 1][y];

		LayoutBlock rightSideBelow = layout[x + 1][y - 1];
		LayoutBlock rightSide = layout[x + 1][y];

		if (x > 1) {
			if (/*leftSideBelow == brick ||*/ brickO[x - 1][y - 1] || leftSideBelow == concrete || leftSideBelow == ladder || leftSide == pole) {
				if (y >= runnerY) {
					break;
				}
			}
		}

		if (x < 28) {
			if (/*rightSideBelow == brick ||*/ brickO[x + 1][y - 1] || rightSideBelow == concrete || rightSideBelow == ladder || rightSide == pole) {
				if (y >= runnerY) {
					break;
				}
			}
		}
	}

	int curRating = 255;

	if (y == runnerY) {
		curRating = abs(curX - x);			// update best rating and direct.
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

void Enemy::Move() {
	actualSpeed = charSpeed * GameTime::getSpeed();

	switch (state) {
	case freeRun:
		FreeRun();
		break;
	case startingToFall:
		StartingToFall();
		break;
	case falling:
		Falling();
		break;
	case digging:
		Digging();
		break;
	case pitting:
		Pitting();
		break;
	case dying:
		Dying();
		break;
	}	
}

void Enemy::Animate() {
	switch (state) {
	case freeRun:
		AnimateFreeRun();
		break;
	case startingToFall:
		AnimateFalling();
		break;
	case falling:
		AnimateFalling();
		break;
	case digging:
		AnimateDigging();
		break;
	case pitting:
		AnimatePitting();
		break;
	case dying:
		AnimateDying();
		break;
	}
}

void Enemy::StartingToFall() {
	//falling back to pit, when enemy did not come out from it!
	if (Pos.x == curX) {
		//std::cout << "\n exiting stf";
		state = falling;
		return;
	}

	//falling from edge left->right
	else if (Pos.x < curX) {
		direction = right;

		if (Pos.x + actualSpeed > curX) {
			dPos.x = curX - Pos.x;
			state = falling;
		}
		else {
			dPos.x = actualSpeed;
		}			
	}
	//falling from edge right->left
	else  {
		direction = left;

		if (Pos.x - actualSpeed < curX) {
			dPos.x = curX - Pos.x;
			state = falling;
		}
		else {
			dPos.x = -actualSpeed;
		}
	}

	dPos.y = -actualSpeed;
}

void Enemy::Digging() {
	
}
//------FREERUN------

//To properly use ladder characters need to be adjusted into center!
void Enemy::LadderTransformation() {
	/*if (middle == ladder && dPos.y > 0) {
		dPos.y = 0;
	}*/

	//you can move on a ladder 1.5 unit vertically, middle block only covers the bottom 2/3 of the ladder, tophalfladder covers the upper 2/3 of ladder
	LayoutBlock topHalfOfLadder = layout[curX][int(Pos.y)];

	if (dPos.y > 0 && middle != ladder && topHalfOfLadder != ladder) {
		dPos.y = 0;
	}

	if (dPos.y < 0 && middle != ladder && downBlock != ladder && middle != pole) {
		dPos.y = 0;
	}

	//Handle when multiple buttons pushed (only used by runner)
	if (dPos.x != 0 && dPos.y != 0) {
		if (dPos.y > 0) {
			if (middle == ladder) {
				dPos.x = 0;
			}
			else if (downBlock == ladder) {
				dPos.y = 0;
			}
		}
		else {
			if (middle == ladder) {
				if (downBlock == ladder || downBlock == pole || downBlock == empty || downBlock == trapDoor) {
					dPos.x = 0;
				}
				else {//if (downBlock != ladder) {
					dPos.y = 0;
				}
			}
			else {
				if (downBlock == ladder) {
					dPos.x = 0;
				}
				else {
					//dPos.x = 0;
				}
			}	
		}			
	}

	//adjust character to middle! (used by everyone)
	if (middle == ladder || downBlock == ladder) {
		if (dPos.x != 0 && Pos.y != curY) {
			dPos.x = 0;
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
		}
		else if (dPos.y != 0 && Pos.x != curX) {
			dPos.y = 0;
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
		}
	}
}

//enemy does not go into other enemy!
void Enemy::CheckCollisionWithOthers() {
	bool checkPit = int(Pos.x) != int(Pos.x + dPos.x);

	for (auto& enemy : enemies) {
		if (enemy.get() != this) {
			//stop if other enemy under is moving!
			if (abs(enemy->Pos.x - Pos.x) < 1.0f && enemy->Pos.y == Pos.y - 1 && enemy->dPrevPos.x != 0 && middle != ladder && middle != pole && enemy->state != pitting) {
				dPos.x = 0;
				return;
			}

			//check X collision
			if (dPos.x != 0 && abs(enemy->Pos.x - (Pos.x + dPos.x)) < 1.0f && abs(Pos.y - enemy->Pos.y) < 1.0f) {	
				//stop if enemy is coming out from hole
				if (checkPit) {
					if (enemy->state == pitting && enemy->pitState == climbing) {
						dPos.x = curX - Pos.x;
						break;
					}
				}

				//when going to same ladder from different sides, who goes from right to left, stop!
				if (-directionX == enemy->directionX) {
					if (directionX == -1) {
						dPos.x = 0;
					}
				}
				//do not go inside other enemy
				else {
					float enemyDiff = enemy->Pos.x - Pos.x;
					short enemyDirectionX = (enemyDiff > 0) - (enemyDiff < 0);
					if (enemyDirectionX == directionX) {
						dPos.x = 0;
					}
				}
			}

			//check Y collision
			if (dPos.y != 0 && abs(enemy->Pos.y - (Pos.y + dPos.y)) < 1.0f && abs(Pos.x - enemy->Pos.x) < 1.0f) {
				//when exiting the same ladder from different sides, who goes from up to down, stop!
				if (-directionY == enemy->directionY) {
					if (directionY == -1) {
						dPos.y = 0;
					}	
				}
				//do not go inside other enemy
				else {
					float enemyDiff = enemy->Pos.y - Pos.y;
					short enemyDirectionY = (enemyDiff > 0) - (enemyDiff < 0);
					if (enemyDirectionY == directionY) {
						dPos.y = 0;
					}
				}
			}
		}
	}
}

void Enemy::CheckCollisionsWithEnvironment() {	
	if (dPos.x != 0) {
		short directionX = (dPos.x > 0) - (dPos.x < 0);
		int nextX = curX + directionX;
		LayoutBlock nextXBlock = layout[nextX][curY];

		if ((nextXBlock == brick || nextXBlock == concrete || nextXBlock == trapDoor) && abs(nextX - Pos.x - dPos.x) < 1.0f) {
			dPos.x = curX - Pos.x;
		}
	}	

	if (dPos.y != 0) {
		short directionY = (dPos.y > 0) - (dPos.y < 0);
		int nextY = curY + directionY;
		LayoutBlock nextYBlock = layout[curX][nextY];

		if (dPos.y > 0) {
			if ((nextYBlock == brick || nextYBlock == concrete || nextYBlock == trapDoor) && abs(nextY - Pos.y - dPos.y) < 1.0f) {
				dPos.y = curY - Pos.y;
			}
		}
		else {
			if ((nextYBlock == brick || nextYBlock == concrete) && abs(nextY - Pos.y - dPos.y) < 1.0f) {
				dPos.y = curY - Pos.y;
			}
		}
	}	
}

void Enemy::FreeRun() {
	LadderTransformation();

	directionX = (dPos.x > 0) - (dPos.x < 0);
	directionY = (dPos.y > 0) - (dPos.y < 0);

	CheckCollisionWithOthers();
	CheckCollisionsWithEnvironment();

	int nextY = int(Pos.y + 0.5 + dPos.y);
	LayoutBlock nextYBlock = layout[curX][nextY];

	//fall from ladder to empty, pole or trapDoor
	if (dPos.y < 0 && middle == ladder && (nextYBlock == empty || nextYBlock == pole || nextYBlock == trapDoor)) {
		InitiateFallingStart();
		return;
	}

	//fall from pole
	if (dPos.y < 0 && middle == pole && (downBlock == empty || downBlock == pole || downBlock == trapDoor)) {
		if (EnemyChecker(curX, curY - 1)) {
			dPos.y = 0;
		}
		else {
			Pos.x = curX;
			dPos.x = 0;
			InitiateFallingStart();
			return;
		}
	}

	int nextX = int(Pos.x + 0.5 + dPos.x);

	bool enemyUnder = false;
	bool enemyNextUnder = false;

	for (auto& enemy : enemies) {
		if (enemy.get() != this) {
			if (enemy->curX == curX && enemy->curY == curY - 1) {
				enemyUnder = true;
			}

			if (directionX != 0 && enemy->curX == nextX && enemy->curY == curY - 1) {
				enemyNextUnder = true;
			}
		}
	}

	//character has to fall when an other character which was under him before, leaves the block under
	if ((middle == empty || middle == trapDoor) && (downBlock == empty || downBlock == pole || downBlock == trapDoor) && !enemyUnder) {
		InitiateFallingStart();
		return;
	}
	
	LayoutBlock nextXBlock = layout[nextX][curY];
	//used to check next block for starting to fall
	LayoutBlock nextUnderBlock = layout[nextX][curY - 1];

	//falling from the edge of a block (brick, concrete, enemy, ladder)
	if (dPos.x != 0 && nextXBlock == empty && (nextUnderBlock == empty || nextUnderBlock == trapDoor || nextUnderBlock == pole) && !enemyNextUnder) {
		InitiateFallingStart();
		return;
	}

	//if some probem arised and character is stuck in air, unstuck it!
	if (Pos.x != curX && Pos.y != curY) {
		bool correct = true;

		for (auto& enemy : enemies) {
			if (enemy.get() != this) {
				if (abs(enemy->Pos.x - Pos.x) < 1.0f && Pos.y < enemy->Pos.y + 1.0f) {
					correct = false;
					break;
				}
			}
		}

		if (correct) {
			InitiateFallingStart();
		}
	}

	CheckGoldCollect();
	CheckGoldDrop();
}

void Enemy::InitiateFallingStart() {
	/*if (Pos.x > curX) {
		direction = left;
	}
	else {
		direction = right;
	}*/

	state = startingToFall;
}

void Enemy::InitiateFallingStop() {
	dPos.y = int(Pos.y + dPos.y) + 1 - Pos.y;
	state = freeRun;
}

bool Enemy::CheckHole() {
	if (layout[curX][curY] == empty && brickO[curX][curY] && dPos.x == 0 && Pos.y > curY) {
		this->holeBrick = brickO[curX][curY].get();
		return true;
	}

	return false;
}

void Enemy::Falling() {
	dPos.x = 0;
	dPos.y = -actualSpeed;

	if (CheckHole()) {
		state = pitting;
		pitState = fallingToPit;
		return;
	}

	//falling into pit from falling
	int nextY = int(Pos.y + dPos.y);

	LayoutBlock blockUnderFallingEnemy = layout[curX][nextY];
	//Falling to brick, concrete, ladder, enemy
	if (blockUnderFallingEnemy == brick || blockUnderFallingEnemy == concrete || blockUnderFallingEnemy == ladder) {
		InitiateFallingStop();
		return;
	}

	//falling onto enemy!
	for (auto& enemy : enemies) {
		if (enemy.get() != this) {
			if (curX == enemy->curX && enemy->Pos.y < Pos.y && abs((Pos.y + dPos.y) - enemy->Pos.y) < 1.0f && layout[enemy->curX][enemy->curY] != pole) {
				InitiateFallingStop();
				return;
			}
		}
	}

	//falling to pole
	if (layout[curX][curY] == pole && Pos.y + dPos.y <= curY && Pos.y > curY) {
		InitiateFallingStop();
		return;
	}

	CheckGoldCollect();
}

void Enemy::Die() {
	Enemy::killCounter++;

	if (carriedGold) {
		carriedGold.reset();
		if (!Enemy::HasGold() && Gold::GetUncollectedSize() == 0) {
			Audio::SFX[4].PlayPause();
			play->generateFinishingLadders();
		}
	}

	std::vector<int> nonEmptyBlocks;
	int vertical = 15;

	//finding a row with an empty block starting from the 15th row, and going down
	while (true) {
		for (int i = 1; i < 29; i++) {
			bool checkGold = Gold::GoldChecker(i, vertical);

			if (layout[i][vertical] == empty && !EnemyChecker(i, vertical) && !checkGold && !brickO[i][vertical]) {
				nonEmptyBlocks.push_back(i);
			}
		}

		if (nonEmptyBlocks.size() != 0) {
			break;
		}

		vertical--;
	}

	Pos.y = vertical;
	short randomX = rand() % nonEmptyBlocks.size();
	Pos.x = nonEmptyBlocks[randomX];
	dPos.x = 0;
	dPos.y = 0;
	state = dying;
	pitState = fallingToPit;
	holeBrick = nullptr;

	dieTimer = gameTime;
}

void Enemy::Dying() {
	dPos.x = 0;
	dPos.y = 0;

	if ((gameTime - dieTimer) > 1) {
		state = falling;
	}
}

void Enemy::Pitting() {
	switch (pitState) {
	case fallingToPit:
		FallingToPit();
		break;
	case moving:
		MovingInPit();
		break;
	case climbing:
		Climbing();
		break;
	}	
}

void Enemy::FallingToPit() {
	if (Pos.x != holeBrick->getPosition().x) {

		//falling from edge left->right
		if (Pos.x < holeBrick->getPosition().x) {
			direction = right;

			if (Pos.x + actualSpeed > holeBrick->getPosition().x) {
				dPos.x = holeBrick->getPosition().x - Pos.x;
			}
			else {
				dPos.x = actualSpeed;
			}
		}
		//falling from edge right->left
		else {
			direction = left;

			if (Pos.x - actualSpeed < holeBrick->getPosition().x) {
				dPos.x = holeBrick->getPosition().x - Pos.x;
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
	if (layout[holeBrick->getPosition().x][holeBrick->getPosition().y] == brick && abs(Pos.y - holeBrick->getPosition().y) >= 0.75) {
		state = freeRun;
		pitState = fallingToPit;
		Pos.y = holeBrick->getPosition().y + 1;

		if (layout[holeBrick->getPosition().y][holeBrick->getPosition().y + 1] == brick) {	
			Die();
		}
		else {
			state = freeRun;
		}

		holeBrick = nullptr;
		dPos.y = 0;
		return;
	}

	dPos.y = -actualSpeed;	

	Vector2DInt brickPosition = holeBrick->getPosition();

	if (int(Pos.y + dPos.y) < brickPosition.y) {
		dPos.y = brickPosition.y - Pos.y;
		pitState = moving;
		holeTimer = gameTime;

		if (carriedGold) {
			carriedGold->SetPos({ float(brickPosition.x), float(brickPosition.y + 1.0f) });
			Gold::addGoldToUncollected(std::move(carriedGold));
		}
	}
}

void Enemy::MovingInPit() {
	//being idle in the hole for a moment
	if (gameTime - holeTimer < holeIdle) {
		dPos.x = 0;
		dPos.y = 0;
	}

	//moving horizontally in hole after being idle
	else if (holeIdle < gameTime - holeTimer && gameTime - holeTimer < holeIdle + holeHorizontalTime) {
		//"harmonic motion" in pit
		Pos.x = holeBrick->getPosition().x + 0.25f * sin(2 * 3.14159265359f / holeHorizontalTime * (gameTime - (holeIdle + holeTimer)));
		dPos.x = 0;
		dPos.y = 0;
	}

	//going up to the half of the hole block!
	else if (gameTime - holeTimer > holeIdle + holeHorizontalTime) {
		Pos.x = holeBrick->getPosition().x;

		//if higher than 3/4 of the block try climbing
		if (Pos.y + actualSpeed > holeBrick->getPosition().y + 0.75f) {
			Pos.y = holeBrick->getPosition().y + 0.75f;
			dPos.y = 0;

			pitState = climbing;

			if (dPos.x > 0) {
				direction = right;
			}
			if (dPos.x < 0) {
				direction = left;
			}
		}
		else {
			Pos.y += actualSpeed;
			dPos.y = 0;

			for (auto& enemy : enemies) {
				if (enemy.get() != this) {
					if (abs(enemy->Pos.x - holeBrick->getPosition().x) < 0.5f && enemy->Pos.y < holeBrick->getPosition().y + 2) {
						pitState = fallingToPit;
						break;
					}
				}
			}
		}

		dPos.x = 0;
	}
	return;
}

void Enemy::Climbing() {
	//if horizontally stopped, fall back!
	if (dPos.x == 0) {
		//this needed when coming out from hole and next it there is a pole or ladder
		//needed because different pathfinding implemantation
		if (dPos.y != 0) {
			if (direction == left) {
				dPos.x = -actualSpeed;
			}
			if (direction == right) {
				dPos.x = +actualSpeed;
			}
		}
		else {
			//std::cout << "\n falling back dpx0";
			pitState = fallingToPit;
			return;
		}
	}
	
	Vector2DInt brickPosition = holeBrick->getPosition();

	//if direction changed, fall back!
	if ((dPos.x > 0 && direction == left) || (dPos.x < 0 && direction == right)) {
		pitState = fallingToPit;
		return;
	}
	//continue climbing
	else {
		dPos.y = actualSpeed;

		//do not go higher than block
		if (Pos.y + dPos.y > brickPosition.y + 1) {
			dPos.y = brickPosition.y + 1 - Pos.y;
		}

		//fall back if the above brick is rebuilt
		if (layout[brickPosition.x][brickPosition.y + 1] == brick) {
			pitState = fallingToPit;
			return;
		}

		//fallback if exit block is rebuilt on left
		if (dPos.x < 0) {
			if (layout[brickPosition.x - 1][brickPosition.y + 1] == brick) {
				pitState = fallingToPit;
				return;
			}
		}

		//fallback if exit block is rebuilt on right
		else if (dPos.x > 0) {
			if (layout[brickPosition.x + 1][brickPosition.y + 1] == brick) {
				pitState = fallingToPit;
				return;
			}
		}

		//fall back if 'exit block' is occupied by an other enemy (exit block == the block where enemy will got after exiting -  x = hole.x+-1, y = hole.y+1
		for (auto& enemy : enemies) {
			if (dPos.x < 0) {
				//if (holePos.x - 1.55 < enemies[i]->Pos.x && enemies[i]->Pos.x < holePos.x && int(enemies[i]->Pos.y + 0.5) == holePos.y + 1) {
				if (Pos.x - 1 < enemy->Pos.x && enemy->Pos.x < Pos.x && enemy->curY == brickPosition.y + 1) {
					//std::cout << "\n falling back from left to right, because enemy is above!";
					pitState = fallingToPit;
					return;
				}
			}
			else if (dPos.x > 0) {
				//if (holePos.x < enemies[i]->Pos.x && enemies[i]->Pos.x < holePos.x + 1.55 && int(enemies[i]->Pos.y + 0.5) == holePos.y + 1) {
				if (Pos.x < enemy->Pos.x && enemy->Pos.x < Pos.x + 1 && enemy->curY == brickPosition.y + 1) {
					//std::cout << "\n falling back from right to left, because enemy is above!";
					pitState = fallingToPit;
					return;
				}
			}
		}

		//out!!
		if (abs((Pos.x + dPos.x) - brickPosition.x) > 0.55f) {
			//std::cout << "\n out from pit!";
				
			if (dPos.x < 0) {
				Pos.x = brickPosition.x - 0.55f;
				dPos.x = 0;
			}
			else if (dPos.x > 0) {
				Pos.x = brickPosition.x + 0.55f;
				dPos.x = 0;
			}

			dPos.y = 0;

			state = freeRun;
			pitState = fallingToPit;
			holeBrick = nullptr;
		}
	}
}

//-----------------------------------------------------------
//------------------------ANIMATE----------------------------
void Enemy::AnimateFreeRun() {
	//going animation
	if (Pos.x - prevPos.x != 0  && (middle == empty || middle == ladder || middle == trapDoor)) {
		AnimateGoing();
	}
	//on ladder, or coming out from pit
	else if (Pos.y - prevPos.y != 0 && (middle == ladder || downBlock == ladder)) {
		AnimateOnLadder();
	}
	//on pole
	else if (middle == pole && (Pos.x - prevPos.x != 0 || Pos.y - prevPos.y != 0) && curY == Pos.y) {
		AnimateOnPole();
	}
}

void Enemy::AnimateDying() {
	TextureRef = textureMap.death + int(4 * (gameTime - dieTimer)) % 4;
}

void Enemy::AnimateDigging() {
	
}

void Enemy::AnimateFalling() {
	int factor = animationFactor * actualSpeed / GameTime::getSpeed();
	int timeFactor = int(factor * gameTime) % 4;

	TextureRef = textureMap.falling + timeFactor;
}

void Enemy::AnimateGoing() {
	int factor = animationFactor * actualSpeed / GameTime::getSpeed();
	int timeFactor = int(factor * gameTime) % 4;

	TextureRef = textureMap.going + timeFactor;
}

void Enemy::AnimateOnLadder() {
	int factor = animationFactor * actualSpeed / GameTime::getSpeed();
	int timeFactor = int(factor * gameTime) % 4;

	TextureRef = textureMap.ladder + timeFactor;
}

void Enemy::AnimateOnPole() {
	int factor = animationFactor * actualSpeed / GameTime::getSpeed();
	int timeFactor = int(factor * gameTime) % 4;

	TextureRef = textureMap.pole + timeFactor;
}

void Enemy::AnimatePitting() {
	switch (pitState) {
		case fallingToPit:
			AnimateFalling();
			break;

		case moving:
			if (Pos.y - prevPos.y != 0 && gameTime - holeTimer > 1) {
				AnimateOnLadder();
			}
			else {
				TextureRef = textureMap.falling;
			}
			
			break;

		case climbing:
			if (Pos.y - prevPos.y != 0) {
				AnimateOnLadder();
			}
			else {
				AnimateGoing();
			}

			break;
	}
}

void Enemy::CheckGoldCollect() {
	//if not carryiing and there is one, carry it
	if (!carriedGold) {
		if ((carriedGold = Gold::GoldCollectChecker(Pos.x, Pos.y))) {
			carriedGold->SetReleaseCounter(rand() % 26 + 14);
		}		
	}
}

//if already carrying, check to drop
void Enemy::CheckGoldDrop() {
	if (carriedGold) {
		int prevX = int(prevPos.x + 0.5);
		int prevY = int(prevPos.y + 0.5);

		if (int(Pos.x + dPos.x + 0.5) != prevX || int(Pos.y + dPos.y + 0.5) != prevY) {
			if (carriedGold->shouldBeReleased()) {
				bool checkGold = Gold::GoldChecker(prevX, prevY);
				LayoutBlock prevBlock = layout[prevX][prevY];
				LayoutBlock prevBlockUnder = layout[prevX][prevY - 1];

				if (prevBlock == empty && !brickO[prevX][prevY] && !checkGold && (prevBlockUnder == brick || prevBlockUnder == concrete || prevBlockUnder == ladder)) {
					carriedGold->SetPos({ (float)prevX, (float)prevY });
					Gold::addGoldToUncollected(std::move(carriedGold));
				}
			}
		}
	}
}