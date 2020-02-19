struct Vector2D {
	float x;
	float y;
};

Vector2D debugPos;

struct intVector2D {
	int x;
	int y;
};

class Enemy;

class Gold {

	bool collected;
	Enemy* carrier;
	int index;

public:
	Vector2D Pos;
	void Initialize(int, Vector2D);
	static int goldNr;
	static int remainingGoldCount;

}gold[100];

int Gold::goldNr = 0;
int Gold::remainingGoldCount = 0;

void Gold::Initialize(int indexIn, Vector2D PosIn) {
	Pos = PosIn;
	carrier = nullptr;
	collected = false;
	index = indexIn;
}

enum Direction {
	right,
	left
};

enum PitState {
	out,
	fallingToPit,
	moving,
	climbing
};

enum playerState {
	freeRun,
	startingToFall,
	falling,
	digging,
	pitting,
	dying
};

class Enemy {
private:

	void FreeRun();
	void Digging();
	void Dying();
	void Falling();
	void Pitting();
	void StartingToFall();
	void IntoPit();
	void OutFromPit();

	void ScanFloor();
	void ScanDown(int, int);
	void ScanUp(int, int);

	bool EnemyChecker(float, float);

public:
	static bool EnemyCheckerGlobal(float, float);
	static Enemy* enemies;
	
	int index;
	static int enemyNr;
	static Gold* gold;
	Gold* carriedGold;

	float charSpeed;

	Direction direction;
	int TextureRef;
	
	float inHoleTime;
	float dieTimer;

	playerState state;
	PitState pitState;

	Vector2D Pos;
	Vector2D dPos;
	Vector2D prevPos;
	Vector2D dPrevPos;
	intVector2D holePos;
	
	int goldVariable = 0;

	void Initialize(int i, Vector2D);
	void Move();
	void Animation();
	int PathFinding();

	static int bestPath;
	static float bestRating;
	static int GoldChecker(float, float);

	Enemy() {}

}enemies[50];

int Enemy::enemyNr;
Gold* Enemy::gold;
Enemy* Enemy::enemies;
int Enemy::bestPath;
float Enemy::bestRating;

bool Enemy::EnemyChecker(float x, float y) {
	for (int i = 1; i < enemyNr; i++)
		if (i != index && abs(x - enemies[i].Pos.x) < 0.5 && abs(y - enemies[i].Pos.y) < 0.5)
			return true;

	return false;
}

bool Enemy::EnemyCheckerGlobal(float x, float y) {
	for (int i = 1; i < enemyNr; i++)
		if (abs(x - enemies[i].Pos.x) < 0.5 && abs(y - enemies[i].Pos.y) < 0.5)
			return true;

	return false;
}


int Enemy::GoldChecker(float x, float y) {

	for (int i = 0; i < Gold::goldNr; i++)
		if (abs(x - gold[i].Pos.x) < 0.1 && abs(y - gold[i].Pos.y) < 0.1)
			return i;
		
	return -1;
}

void Enemy::Initialize(int i, Vector2D PosIn) {

	index = i;

	Pos.x = PosIn.x;
	Pos.y = PosIn.y;

	prevPos.x = Pos.x;
	prevPos.y = Pos.y;
	
	holePos.x = -1;
	holePos.y = -1;

	dPos.x = 0;
	dPos.y = 0;

	dPrevPos.x = 0;
	dPrevPos.y = 0;

	direction = left;
	pitState = out;
	state = freeRun;

	TextureRef = 12;

	if (i == 0)
		TextureRef = 48;

	carriedGold = nullptr;

	goldVariable = -1;
}

void Enemy::Move() {

	charSpeed = enemySpeed * speed;

	if(index == 0)
		charSpeed = playerSpeed *speed;

	if (index == 0 && state == freeRun) {

		if (leftDigButton.impulse())
			left_pit = true;
		else
			left_pit = false;

		if (rightDigButton.impulse())
			right_pit = true;
		else
			right_pit = false;

		if (right_pit && left_pit) {
			left_pit = false;
			right_pit = false;
		}
	}

	if (index == 1) {
		
		std::cout << "";
		if (Pos.y > 14.53 && Pos.x == int (Pos.x + 0.5)) {
  			std::cout << "";
		}

	}

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

void Enemy::StartingToFall() {

	//falling back to pit, when enemy did not come out from it!
	if (pitState != out) {

		if (Pos.x == holePos.x) {
			state = pitting;
			pitState = fallingToPit;
			dPos.x = 0;
		}

		if (Pos.x > holePos.x) {
			Pos.x -= charSpeed;
			dPos.x = 0;

			if (Pos.x < holePos.x) {
				Pos.x = holePos.x;
				state = pitting;
				pitState = fallingToPit;
				//std::cout << "\n falling back r->l: to pit";
			}
		}

		if (Pos.x < holePos.x) {
			Pos.x += charSpeed;
			dPos.x = 0;

			if (Pos.x > holePos.x) {
				Pos.x = holePos.x;
				state = pitting;
				pitState = fallingToPit;
				//std::cout << "\n falling back l->r: to pit";
			}
		}

		dPos.y = -charSpeed;
		return;
	}

	if (Pos.x == int(Pos.x + 0.5)) {
		
		state = falling;
		return;
	}

	//std::cout << std::endl << index << ". enemy starting horizontally in (not in pit, yet): " << frameCounter;

	//falling from edge left->right
	if (direction == right) {

		if (index > 0 && gameTime > holeTimer[int(Pos.x + 0.5)][int(Pos.y + 0.5) - 1] + 0.5 && holeTimer[int(Pos.x + 0.5)][int(Pos.y + 0.5) - 1] > 0) {
			
			pitState = fallingToPit;

			holePos.x = int(Pos.x + 0.5);
			holePos.y = int(Pos.y + 0.5) - 1;

			//std::cout << std::endl << index <<  ". enemy into a pit,  left->right - x: "  << holePos.x << ", y: " << holePos.y << std::endl;

			return;
		}
		else {
			holePos.x = -1;
			holePos.y = -1;
		}


		if (int(Pos.x + charSpeed) != int(Pos.x)) {
			Pos.x = int(Pos.x + charSpeed);
			state = falling;
		}
		else
			Pos.x += charSpeed;
	}

	//falling from edge right->left
	if (direction == left) {

		if (index > 0 && gameTime > holeTimer[int(Pos.x + 0.5)][int(Pos.y + 0.5) - 1] + 0.5 && holeTimer[int(Pos.x + 0.5)][int(Pos.y + 0.5) - 1] > 0) {
			
			pitState = fallingToPit;

			holePos.x = int(Pos.x + 0.5);
			holePos.y = int(Pos.y + 0.5) - 1;

			//std::cout << std::endl << index << ". enemy into a pit,  right->left - x: " << holePos.x << ", y: " << holePos.y << std::endl;

			return;
		}
		else {
			holePos.x = -1;
			holePos.y = -1;
		}

		if (int(Pos.x - charSpeed) != int(Pos.x)) {
			Pos.x = int(Pos.x - charSpeed) + 1;
			state = falling;
		}
		else
			Pos.x -= charSpeed;
	}

	Pos.y -= charSpeed;
	dPos.y = 0;
	dPos.x = 0;
}

void Enemy::Digging() {
	
	if (left_pit || right_pit) {	
		//Position runner to middle and hold him in place
		//if (hangok[1].playControl) {
		if (Audio::SFX[1].GetPlayStatus() == playing) {
			//beigazitas folyamatosan

			if (Pos.x - int(Pos.x) < 0.5) {
				if (Pos.x - playerSpeed * speed - int(Pos.x - playerSpeed * speed) > 0.5) {
					Pos.x = int(Pos.x - playerSpeed * speed) + 1;
					dPos.x = 0;
				}
				else
					dPos.x = -playerSpeed * speed;
			}

			else {
				if (Pos.x + playerSpeed * speed - int(Pos.x + playerSpeed * speed) < 0.5) {
					Pos.x = int(Pos.x + playerSpeed * speed);
					dPos.x = 0;
				}
				else
					dPos.x = playerSpeed * speed;
			}

			int curY = int(Pos.y + 0.5);

			if (Pos.y > curY) {
				if (Pos.y - charSpeed < curY)
					Pos.y = curY;
				else
					Pos.y -= charSpeed;
			}

			if (Pos.y < curY) {
				if (Pos.y + charSpeed > curY)
					Pos.y = curY;
				else 
					Pos.y += charSpeed;
			}

			dPos.y = 0;
		}
		else {
			left_pit = false;
			right_pit = false;
			dPos.x = 0;
			dPos.y = 0;
			state = freeRun;
		}

	}
}

void Enemy::FreeRun() {

	int curX = int(Pos.x + 0.5);
	int leftX = curX - 1;
	int rightX = curX + 1;

	int curY = int(Pos.y + 0.5);
	int upY = curY + 1;
	int downY = curY - 1;

	layoutBlock middle = layout[curX][curY];
	layoutBlock leftBlock = layout[leftX][curY];
	layoutBlock rightBlock = layout[rightX][curY];

	layoutBlock upBlock = layout[curX][upY];
	layoutBlock upLeftBlock = layout[leftX][upY];
	layoutBlock upRightBlock = layout[rightX][upY];

	layoutBlock downBlock = layout[curX][downY];
	layoutBlock downLeftBlock = layout[leftX][downY];
	layoutBlock downRightBlock = layout[rightX][downY];

	//digging
	if (index == 0) {

		layoutBlock nextBlock = empty;
		layoutBlock downNextBlock = empty;

		int nextX = curX;

		if (left_pit) {

			nextBlock = leftBlock;
			downNextBlock = downLeftBlock;
			nextX = leftX;
		}

		if (right_pit) {

			nextBlock = rightBlock;
			downNextBlock = downRightBlock;
			nextX = rightX;
		}

		if (left_pit || right_pit) {

			int checkGold = Enemy::GoldChecker(nextX, curY);

			if (downNextBlock == brick && nextBlock == empty && checkGold < 0) {
				digTime = gameTime;

				Audio::SFX[1].PlayPause();

				holeTimer[nextX][downY] = gameTime;
				randomDebris = rand() % 3;
				state = digging;
				dPos.x = 0;
				dPos.y = 0;
			}
		}
	}

	//enemy does not go into other enemy!
	if (index != 0)
		for (int i = 1; i < enemyNr; i++) {

			if (i != index && abs(Pos.y - enemies[i].Pos.y) < 1.0f && abs(enemies[i].Pos.x - (Pos.x + dPos.x)) < 1.0f) {

				//left->right
				if (dPos.x > 0 && enemies[i].Pos.x > Pos.x) {

					//condition to place two enemies next to each other when one stopped and the other want to go into him
					//this positioning needed when two guards want to climb down the same ladder from different sides
					if (enemies[i].dPrevPos.x == 0 && enemies[i].dPrevPos.y == 0 && enemies[i].state != pitting && Pos.y == int(Pos.y + 0.5))
						Pos.x = enemies[i].Pos.x - 1;

					dPos.x = 0;

					if (middle == ladder || (downBlock == ladder && Pos.y - (int(Pos.y + 0.5) - 1) < 1.0)) {

						if (Pos.y != curY) {
							if (Pos.y > curY) {

								Pos.y -= charSpeed;
								if (Pos.y < curY)
									Pos.y = curY;
							}

							if (Pos.y < curY) {

								Pos.y += charSpeed;
								if (Pos.y > curY)
									Pos.y = curY;
							}
						}

						dPos.y = 0;
					}
				}

				//right->left
				if (dPos.x < 0 && Pos.x > enemies[i].Pos.x) {

					//see explanation above
					if (enemies[i].dPrevPos.x == 0 && enemies[i].dPrevPos.y == 0 && enemies[i].state != pitting && Pos.y == int(Pos.y + 0.5))
						Pos.x = enemies[i].Pos.x + 1;

					dPos.x = 0;

					if (middle == ladder || (downBlock == ladder && Pos.y - (int(Pos.y + 0.5) - 1) < 1.0)) {

						if (Pos.y != curY) {
							if (Pos.y > curY) {

								Pos.y -= charSpeed;
								if (Pos.y < curY)
									Pos.y = curY;
							}

							if (Pos.y < curY) {

								Pos.y += charSpeed;
								if (Pos.y > curY)
									Pos.y = curY;
							}
						}

						dPos.y = 0;
					}

				}
			}

			//while laddering
			if (i != index && abs(Pos.x - enemies[i].Pos.x) < 1.0f && abs(enemies[i].Pos.y - (Pos.y + dPos.y)) < 1.0f) {

				//down->up
				if (dPos.y > 0 && enemies[i].Pos.y > Pos.y) {

					//probably this condition needed too!
					if (enemies[i].dPrevPos.x == 0 && enemies[i].dPrevPos.y == 0 && enemies[i].state != pitting && Pos.x == int(Pos.x + 0.5))
						Pos.y = enemies[i].Pos.y - 1;

					dPos.y = 0;

					if (middle == ladder || (downBlock == ladder && Pos.y - (int(Pos.y + 0.5) - 1) < 1.0)) {
						if (Pos.x > curX) {

							Pos.x -= charSpeed;
							if (Pos.x < curX)
								Pos.x = curX;
						}

						if (Pos.x < curX) {

							Pos.x += charSpeed;
							if (Pos.x > curX)
								Pos.x = curX;
						}

						dPos.x = 0;
					}
				}

				//up->down
				if (dPos.y < 0 && Pos.y > enemies[i].Pos.y) {

					if (enemies[i].dPrevPos.x == 0 && enemies[i].dPrevPos.y == 0 && enemies[i].state != pitting && Pos.x == int(Pos.x + 0.5))
						Pos.y = enemies[i].Pos.y + 1;

					dPos.y = 0;

					if (middle == ladder) {
						if (Pos.x > curX) {

							Pos.x -= charSpeed;
							if (Pos.x < curX)
								Pos.x = curX;
						}

						if (Pos.x < curX) {

							Pos.x += charSpeed;
							if (Pos.x > curX)
								Pos.x = curX;
						}

						dPos.x = 0;
					}

					if ((downBlock == ladder || downBlock == pole || downBlock == empty)) {

						if (Pos.x > curX) {

							Pos.x -= charSpeed;
							if (Pos.x < curX)
								Pos.x = curX;
						}

						if (Pos.x < curX) {

							Pos.x += charSpeed;
							if (Pos.x > curX)
								Pos.x = curX;
						}

						dPos.x = 0;
					}

				}
			}

			//stop if other enemy under is moving!
			if (i != index && abs(enemies[i].Pos.x - Pos.x) < 1.0f && enemies[i].Pos.y == Pos.y - 1 && enemies[i].dPrevPos.x != 0 && middle != ladder && middle != pole && enemies[i].state != pitting)
				dPos.x = 0;

		}

	//enemy stop if other enemy is coming out from next hole!
	if (index != 0 && dPos.x != 0) {

		int curX = int(Pos.x + 0.5);
		int curY = int(Pos.y + 0.5);

		int nextX = curX;

		bool checkPit = false;

		if (Pos.x <= curX && Pos.x + dPos.x > curX) {
			nextX = curX + 1;
			checkPit = true;
		}

		if (Pos.x >= curX && Pos.x + dPos.x < curX) {
			nextX = curX - 1;
			checkPit = true;
		}

		if (checkPit)
			for (int i = 1; i < enemyNr; i++) {
				if (i != index) {
					if (enemies[i].holePos.x == nextX && enemies[i].holePos.y == curY - 1 && enemies[i].pitState == climbing) {

						//std::cout << std::endl << i << ". enemy is in hole in front of: " << index;

						Pos.x = curX;
						dPos.x = 0;
						break;
					}
				}
			}
	}

	//collide with brick, concrete, trapDoor from side
	layoutBlock nextXBlock = empty;

	int nextRightX = int(Pos.x + dPos.x) + 1;
	int nextLeftX = int(Pos.x + dPos.x);

	layoutBlock nextRightBlock = layout[nextRightX][curY];
	layoutBlock nextLeftBlock = layout[nextLeftX][curY];

	int nextXPosition = Pos.x;

	if (dPos.x > 0) {
		nextXBlock = nextRightBlock;
		nextXPosition = nextRightX - 1;
	}

	else if (dPos.x < 0) {
		nextXBlock = nextLeftBlock;
		nextXPosition = nextLeftX + 1;
	}

	if (nextXBlock == brick || nextXBlock == concrete || nextXBlock == trapDoor) {

		Pos.x = nextXPosition;
		dPos.x = 0;

		if (middle == ladder && dPos.y == 0)
			if (Pos.y != curY) {
				if (Pos.y > curY) {

					Pos.y -= charSpeed;
					if (Pos.y < curY)
						Pos.y = curY;
				}

				if (Pos.y < curY) {

					Pos.y += charSpeed;
					if (Pos.y > curY)
						Pos.y = curY;
				}
			}
	}

	int nextUpY = int(Pos.y + dPos.y) + 1;
	int nextDownY = int(Pos.y + dPos.y);

	layoutBlock nextYBlock = empty;

	layoutBlock nextUpBlock = layout[curX][nextUpY];
	layoutBlock nextDownBlock = layout[curX][nextDownY];

	int nextYPosition = Pos.y;

	if (dPos.y < 0) {
		nextYBlock = nextDownBlock;
		nextYPosition = nextDownY + 1;
	}

	if (dPos.y > 0) {
		nextYBlock = nextUpBlock;
		nextYPosition = nextUpY - 1;
	}

	//from ladder to next block
	if ((middle == ladder)) {

		if (Pos.y + dPos.y > curY)
			if (nextYBlock == brick || nextYBlock == concrete || nextYBlock == trapDoor) {

				if (Pos.y != curY)
					dPos.x = 0;

				if (Pos.x != curX && dPos.x == 0) {

					if (Pos.x > curX) {

						Pos.x -= charSpeed;
						if (Pos.x < curX)
							Pos.x = curX;
					}

					if (Pos.x < curX) {

						Pos.x += charSpeed;
						if (Pos.x > curX)
							Pos.x = curX;
					}

					dPos.x = 0;
				}

				Pos.y = curY;
				dPos.y = 0;
			}

		if (Pos.y + dPos.y < curY)
			if (nextYBlock == brick || nextYBlock == concrete) {

				if (Pos.y != curY)
					dPos.x = 0;

				if (Pos.x != curX && dPos.x == 0) {

					if (Pos.x > curX) {

						Pos.x -= charSpeed;
						if (Pos.x < curX)
							Pos.x = curX;
					}

					if (Pos.x < curX) {

						Pos.x += charSpeed;
						if (Pos.x > curX)
							Pos.x = curX;
					}

					dPos.x = 0;
				}

				Pos.y = curY;
				dPos.y = 0;
			}
	}

	int yBelowLadder = int(Pos.y + 0.5 + dPos.y);
	layoutBlock blockBelowLadder = layout[curX][yBelowLadder];

	//fall from ladder to empty, pole or trapDoor
	if (middle == ladder && (blockBelowLadder == empty || blockBelowLadder == pole || blockBelowLadder == trapDoor) && dPos.y < 0) {

		std::cout << "\n fall to empty!";

		state = falling;
		if (index == 0)
			Audio::SFX[17].PlayPause();
		dPos.y = 0;
	}

	int yBelowPole = int(Pos.y);
	int yAbovePole = int(Pos.y + 0.5 + dPos.y);
	layoutBlock blockBelowPole = layout[curX][yBelowPole];
	layoutBlock enemyOnBlockAbovePole = layout[curX][yAbovePole];

	//fall from pole, the block above pole statement is needed to rule out the case when runner is on a ladder which is above the pole
	if (middle == pole && dPos.y < 0 && (nextYBlock == empty || nextYBlock == pole || nextYBlock == trapDoor) && (enemyOnBlockAbovePole != ladder) && !EnemyChecker(curX, nextDownY)) {

		state = falling;
		if (index == 0)
			Audio::SFX[17].PlayPause();

		Pos.x = curX;
		dPos.x = 0;
		dPos.y = 0;
	}

	layoutBlock blockBelowUsedLadder = layout[curX][int(Pos.y - 0.5)];
	layoutBlock curLadder = layout[curX][int(Pos.y)];
	layoutBlock nextLadder = layout[curX][int(Pos.y + dPos.y)];

	//ladder interactions

	//at the bottom of the ladder go anywhere
	if (middle == ladder && blockBelowUsedLadder != ladder)
		;

	//in the middle of the ladder go anywhere
	else if (nextLadder == ladder)
		;

	//at the top of the ladder don't go up
	else if (curLadder == ladder && nextLadder != ladder && dPos.y > 0) {
		Pos.y = nextUpY - 1;
		dPos.y = 0;
		dPos.x = 0;
	}

	//not on a ladder
	else
		dPos.y = 0;

	//Positioning character x pos to integer first, when trying to go to up or down on ladder
	if (((middle == ladder || downBlock == ladder) || (middle == ladder && blockBelowUsedLadder != ladder)) && dPos.y != 0) {

		if (Pos.x != curX) {

			if (Pos.x > curX) {

				Pos.x -= charSpeed;
				if (Pos.x < curX)
					Pos.x = curX;
			}

			if (Pos.x < curX) {

				Pos.x += charSpeed;
				if (Pos.x > curX)
					Pos.x = curX;
			}

			dPos.y = 0;
		}

		dPos.x = 0;
	}

	//Positioning character y pos to integer first, when trying to go to left ot right on ladder
	if ((curLadder == ladder || (middle == ladder && blockBelowUsedLadder != ladder)) && dPos.x != 0) {

		if (Pos.y != curY) {
			if (Pos.y > curY) {

				Pos.y -= charSpeed;
				if (Pos.y < curY)
					Pos.y = curY;
			}

			if (Pos.y < curY) {

				Pos.y += charSpeed;
				if (Pos.y > curY)
					Pos.y = curY;
			}

			dPos.x = 0;
		}

		dPos.y = 0;
	}

	//used to check next block for starting to fall
	int nextXPos = int(Pos.x + 0.5 + dPos.x);

	layoutBlock nextUnderBlock = layout[nextXPos][downY];
	layoutBlock nextX = layout[nextXPos][curY];

	//character has to fall when an other character which was under him before, leaves the block under
	if ((middle == empty || middle == trapDoor) && (downBlock == empty || downBlock == pole || downBlock == trapDoor) /*&& dPos.x == 0*/) {

		bool fall = true;
		for (int i = 1; i < enemyNr; i++) {
			if (index != i) {
				if (enemies[i].holePos.x == curX && enemies[i].holePos.y == curY - 1 && (enemies[i].pitState != out)) {

					fall = false;
					break;
				}

				if (curY == int(enemies[i].Pos.y + 0.5f) + 1 && curX == int(enemies[i].Pos.x + 0.5f)) {

					fall = false;
					break;
				}

			}
		}

		if (fall) {

			if (Pos.x != curX) {
				state = startingToFall;

				if (Pos.x > curX)
					direction = left;
				else
					direction = right;

				//std::cout << "\n starting to fall, becaues not in middle!";
			}
			else
				state = falling;

			dPos.x = 0;

			if (index == 0)
				Audio::SFX[17].PlayPause();

			return;
		}
	}

	//falling from the edge of a block (brick, concrete, enemy, ladder)
	if (dPos.x != 0 && nextX == empty && (nextUnderBlock == empty || nextUnderBlock == trapDoor)) {

		bool fall = true;

		for (int i = 1; i < enemyNr; i++)
			if (index != i) {

				if (enemies[i].holePos.x == nextXPos && enemies[i].holePos.y == curY - 1 && (enemies[i].pitState != out)) {
					fall = false;
					break;
				}

				if (abs(enemies[i].Pos.x - (Pos.x + dPos.x)) < 0.5 && int(enemies[i].Pos.y + 0.5) + 1.0 >= Pos.y) {
					fall = false;
					break;
				}
			}

		if (fall) {

			if (Pos.x != curX)
				state = startingToFall;

			if (index == 0)
				Audio::SFX[17].PlayPause();
		}
	}

	//falling from the edge of a block to a pole
	if (layout[nextXPos][curY - 1] == pole && layout[nextXPos][curY] == empty && dPos.x != 0) {

		state = startingToFall;

		if (index == 0)
			Audio::SFX[17].PlayPause();
	}

	//if some probem arised and character is stuck in air, unstuck it!
	if (Pos.x != int(Pos.x + 0.5) && Pos.y != int(Pos.y + 0.5)) {

		bool correct = true;

		for (int i = 1; i < enemyNr; i++)
			if (i != index)
				if (abs(enemies[i].Pos.x - Pos.x) < 1.0f && Pos.y < enemies[i].Pos.y + 1.0f) {

					correct = false;
					break;
				}

		if (correct)
			state = startingToFall;

	}

}

void Enemy::Falling() {
	
	dPos.x = 0;
	dPos.y = -charSpeed;

	//falling into pit from falling
	int curX = int(Pos.x + 0.5);
	int nextY = int(Pos.y + dPos.y);

	layoutBlock blockUnderFallingEnemy = layout[curX][nextY];

	if (index == 0)
		if (layout[curX][int(Pos.y + 0.5)] == trapDoor)
			holeTimer[curX][int(Pos.y + 0.5)] = -2;

	//Falling into pit
	if (blockUnderFallingEnemy == empty && holeTimer[curX][nextY] != 0 && index > 0 && !EnemyChecker(curX, nextY)) {

		holePos.x = curX;
		holePos.y = nextY;
		IntoPit();
		return;
	}

	//Falling to brick, concrete, ladder, enemy
	if (blockUnderFallingEnemy == brick || blockUnderFallingEnemy == concrete || blockUnderFallingEnemy == ladder) {

		Pos.y = nextY + 1;
		dPos.y = 0;			

		state = freeRun;

		if (index == 0) {
			Audio::SFX[17].StopAndRewind();
			idleTime = gameTime - 1;
		}
		
	}
	
	//falling onto enemy!
	for (int i = 1; i < enemyNr; i++)
		if (i != index) {

			if (enemies[i].Pos.y < Pos.y && abs((Pos.y + dPos.y) - enemies[i].Pos.y) < 1.0 && curX == int(enemies[i].Pos.x + 0.5) && layout[int(enemies[i].Pos.x + 0.5)][int(enemies[i].Pos.y + 0.5)] != pole) {

				Pos.y = int(enemies[i].Pos.y + 0.5) + 1;
				state = freeRun;

				if (index == 0) {
					Audio::SFX[17].StopAndRewind();
					idleTime = gameTime - 1;

					if (enemies[i].prevPos.y < 0 && enemies[i].state == freeRun)
						state = dying;
				}

				dPos.y = 0;
			}
		}

	//falling to pole
	if (layout[curX][int(Pos.y + 0.5)] == pole && Pos.y + dPos.y <= int(Pos.y + 0.5) && Pos.y > int(Pos.y + 0.5)) {

		std::cout << "\n to fall stop on pole";

		Pos.y = nextY + 1;
		dPos.y = 0;

		state = freeRun;

		if (index == 0)
			Audio::SFX[17].StopAndRewind();
	}
}

void Enemy::Dying() {

	//finding a row with an empty block starting from the 15th row, and going down
	if (Pos.x == -1) {
		
		int vertical = 15;

		while (true) {

			bool foundEmptyBlockToRespawn = false;
			for (int i = 1; i < 29; i++) {

				int checkGold = Enemy::GoldChecker(i, vertical);

				if (layout[i][vertical] == empty && !EnemyChecker(i, vertical) && checkGold < 0) {
					foundEmptyBlockToRespawn = true;
					break;
				}
			}

			if (foundEmptyBlockToRespawn)
				break;

			vertical--;
		}

		bool reSpawn = false;

		//now a row is found which has an empty block, now randomly found one of it to respawn
		while (!reSpawn) {
			
			int res = rand() % 28 + 1;

			int checkGold = Enemy::GoldChecker(res, vertical);

			if (layout[res][vertical] == empty && !EnemyChecker(res, vertical) && checkGold < 0) {
				
				dieTimer = gameTime;

				Pos.x = res;
				Pos.y = vertical;

				std::cout << "\n respawn ended - Pos.x: " << Pos.x << ", Pos.y: " << Pos.y <<"carriedGold: "<<carriedGold;

				reSpawn = true;
			}
		}
	}
	
	dPos.x = 0;
	dPos.y = 0;

	if ((gameTime - dieTimer) > 1) 
		state = falling;
}

void Enemy::Pitting() {

	//enemy dies in pit
	if (holeTimer[holePos.x][holePos.y] == 0 && abs(Pos.y - holePos.y) < 0.75) {
		if (carriedGold != nullptr) {
			
			carriedGold->Pos = { (float) holePos.x, (float) holePos.y + 1 };
			carriedGold = nullptr;

			std::cout << "\n" << index << ". enemy dies here!";

			goldVariable = -1;
		}

		Pos.x = -1;
		Pos.y = -1;

		dPos.x = 0;
		dPos.y = 0;

		holePos.x = -1;
		holePos.y = -1;

		score_enemy += 300;

		pitState = out;
		state = dying;
	}

	//enemy dying prevented because enemy wasn't deep enough when the brick got full again
	else if (holeTimer[holePos.x][holePos.y] == 0 && abs(Pos.y - holePos.y) >= 0.75) {

		state = freeRun;
		pitState = out;

		Pos.y = holePos.y + 1;
		holePos.x = -1;
		holePos.y = -1;
		dPos.y = 0;
		return;
	}

	//enemy falling to the bottom of the pit
	if (pitState == fallingToPit) {

		dPos.y = -enemySpeed * speed;
		dPos.x = 0;

		if (int(Pos.y + dPos.y) < holePos.y) {
			Pos.y = int(Pos.y);
			dPos.y = 0;
			pitState = moving;
			inHoleTime = gameTime;

			if (carriedGold != nullptr) {

				carriedGold->Pos = { float(holePos.x), float(holePos.y + 1) };

				goldVariable = -1;
				carriedGold = nullptr;
			}
		}
	}

	//enemy moving before climbing up
	else if (pitState == moving) {
		
		//being idle in the hole for a moment
		if (gameTime < inHoleTime + holeIdle) {	
			dPos.x = 0;
			dPos.y = 0;
		}

		//moving horizontally in hole after being idle
		if (inHoleTime + holeIdle < gameTime && gameTime < inHoleTime + holeIdle + holeHorizontalTime) {

			//"harmonic motion" in pit
			Pos.x = holePos.x + 0.25*sin(2 * 3.14159265359 / holeHorizontalTime * (gameTime - (holeIdle + inHoleTime)));
			dPos.x = 0;
			dPos.y = 0;
		}

		//going up to the half of the hole block!
		if (gameTime > inHoleTime + holeIdle + holeHorizontalTime) {
			Pos.x = holePos.x;
			
			//if higher than 3/4 of the block try climbing
			if (Pos.y + charSpeed > holePos.y + 0.75) {
				Pos.y = holePos.y + 0.75;
				dPos.y = 0;

				pitState = climbing;

				if (dPos.x > 0)
					direction = right;
				if (dPos.x < 0)
					direction = left;
					
			}
			else {
				Pos.y += charSpeed;
				dPos.y = 0;	

				for (int i = 1; i < enemyNr; i++)
					if (i != index) {
						if (abs(enemies[i].Pos.x - holePos.x) < 0.5 && enemies[i].Pos.y < holePos.y + 2) {

							IntoPit();
							break;
						}
					}
			}

			dPos.x = 0;
		}
		return;
	}

//enemy climbing up
	else if (pitState == climbing) {

		//if horizontally stopped, fall back!
		
		if (dPos.x == 0) {
						
			//this needed when coming out from hole and next it there is a pole or ladder
			if (dPos.y != 0) {
				if (direction == left)
					dPos.x = -charSpeed;
				if (direction == right)
					dPos.x = +charSpeed;
			}

			else {
				//std::cout << "\n no x-input, fall back!";
				state = startingToFall;
				return;
			}
		}
		
		if (dPos.x != 0) {

			//if direction changed, fall back!
			if ((dPos.x > 0 && direction == left) || (dPos.x < 0 && direction == right))	{

				std::cout << "\N FALL BACK!";

				state = startingToFall;
				return;
			}
			//continue climbing
			else {

				dPos.y = charSpeed;

				//do not go higher than block
				if (Pos.y + dPos.y > holePos.y + 1) {
					dPos.y = 0;
					Pos.y = holePos.y + 1;
				}

				//fall back if the above brick is rebuilt
				if (layout[holePos.x][holePos.y + 1] == brick) {
					state = startingToFall;
					return;
				}
	
				//fallback if exit block is rebuilt on left
				if (dPos.x < 0) {
					if (layout[holePos.x -1 ][holePos.y + 1] == brick) {
						state = startingToFall;
						return;
					}
				}

				//fallback if exit block is rebuilt on right
				if (dPos.x > 0) {
					if (layout[holePos.x + 1][holePos.y + 1] == brick) {
						state = startingToFall;
						return;
					}
				}


				//fall back if 'exit block' is occupied by an other enemy (exit block == the block where enemy will got after exiting -  x = hole.x+-1, y = hole.y+1
				for (int i = 1; i < enemyNr; i++)
					if (i != index) {

						if (dPos.x < 0) {

							//if (holePos.x - 1.55 < enemies[i].Pos.x && enemies[i].Pos.x < holePos.x && int(enemies[i].Pos.y + 0.5) == holePos.y + 1) {
							if (Pos.x - 1 < enemies[i].Pos.x && enemies[i].Pos.x < Pos.x && int(enemies[i].Pos.y + 0.5) == holePos.y + 1) {
								std::cout << "\n falling back from left to right, because enemy is above!";
								state = startingToFall;
								return;
							}
						}

						if (dPos.x > 0) {

							//if (holePos.x < enemies[i].Pos.x && enemies[i].Pos.x < holePos.x + 1.55 && int(enemies[i].Pos.y + 0.5) == holePos.y + 1) {
							if (Pos.x < enemies[i].Pos.x && enemies[i].Pos.x < Pos.x + 1 && int(enemies[i].Pos.y + 0.5) == holePos.y + 1) {
								std::cout << "\n falling back from right to left, because enemy is above!";
								state = startingToFall;
								return;
							}
						}
					}

				//out!!
				if (abs((Pos.x + dPos.x) - holePos.x) > 0.55) {

					if (dPos.x < 0) {
						Pos.x = holePos.x - 0.55;
						dPos.x = 0;
						//std::cout << "\n exit to left!";
					}

					if (dPos.x > 0) {
						Pos.x = holePos.x + 0.55;
						dPos.x = 0;
						//std::cout << "\n exit to right!";
					}

					dPos.y = 0;

					OutFromPit();
					holePos.x = -1;
					holePos.y = -1;
				}

			}
		}
	}
}

void Enemy::Animation() {

	charSpeed = enemySpeed * speed;

	if (index == 0)
		charSpeed = playerSpeed * speed;


	//determining the direction of the character for animation (and pitting too)
	if (state != falling) {
		if (prevPos.x - Pos.x > 0)
			direction = left;
		if (prevPos.x - Pos.x < 0)
			direction = right;
	}

	//az álló animációhoz egy timer kezdõ idõpontja, pontosabban az utolso olyan pillanat amikor meg nem allt, és ahhoz viszonyítunk
	if(index == 0)
		if (prevPos.x - Pos.x != 0) {
			idleTime = gameTime;
		}

	int curX = int(Pos.x + 0.5);
	int curY = int(Pos.y + 0.5);

	layoutBlock middle = layout[curX][curY];
	layoutBlock downBlock = layout[curX][curY - 1];

	//digging animation
	if (index == 0 && state == digging && middle != pole) {
		
		//on Ladder
		if (middle == ladder) {
			direction = left;
			TextureRef = 36;
			idleTime = -1;
		}
		else {

			if (left_pit) {
				direction = left;
				TextureRef = 24;
			}

			if (right_pit) {
				direction = right;
				TextureRef = 25;
			}

			idleTime = -1;
			return;
		}		
	}

	//factor to slow or fasten animationSpeed
	int animationFactor = 20;

	//dying
	if (state == dying) {
		if (index == 0)
			return;

		TextureRef = 8 + int(4 * (gameTime - dieTimer)) % 4;
		return;
	}

	//runner idle animation
	if(index == 0)
		if (Pos.x - prevPos.x == 0 && Pos.y - prevPos.y == 0 && idleTime>0 && gameTime > 1 + idleTime && (((middle == empty || middle == trapDoor) && Pos.y == curY) || (middle == ladder && Pos.x != curX))) {

			TextureRef = 27;

			if (int(gameTime) % 2 == 0)
				TextureRef = 26;
		}

	//falling animation
	if(enemies[0].state != dying)
		if ((state == falling || state == startingToFall || (state == pitting && pitState == fallingToPit && Pos.y != prevPos.y))) {

			//std::cout << "\n falling: " << index;

			int factor = animationFactor * charSpeed / speed;

			int timeFactor = int(factor * gameTime) % 4;
			int enemyRef = 16;

			if (index == 0)
				enemyRef = 52;

			TextureRef = enemyRef + timeFactor;
		}

	//going animation
	if(Pos.x - prevPos.x != 0 && (state == freeRun || pitState == climbing) && (middle == empty || middle == ladder || middle == trapDoor)) {
		
		int factor = animationFactor * charSpeed / speed;
		int timeFactor = int(factor * gameTime) % 4;

		int enemyRef = 12;
		if (index == 0)
			enemyRef = 48;

		TextureRef = enemyRef + timeFactor;

		if (index == 0) {

			if (Audio::SFX[9 + going[0]].GetPlayStatus() == stopped) {

				Audio::SFX[10 - going[0]].PlayPause();
				going[0] = 1 - going[0];
				
			}
		}			
	}

	//on ladder, or coming out from pit
	if ((Pos.y - prevPos.y != 0 && (state == freeRun && (middle == ladder || downBlock == ladder))) || (Pos.y - prevPos.y > 0 && Pos.x - prevPos.x == 0 && (pitState == climbing || pitState == moving)  )) {

		int factor = animationFactor * charSpeed / speed;

		int timeFactor = int(factor * gameTime) % 4;
		int enemyRef = 0;
		if (index == 0)
			enemyRef = 36;

		TextureRef = enemyRef + timeFactor;

		if (index == 0)
			if (Audio::SFX[11 + going[1]].GetPlayStatus() == stopped) {
				Audio::SFX[12 - going[1]].PlayPause();
				going[1] = 1 - going[1];
			}
	}

	//on pole
	if (state == freeRun && middle == pole && (Pos.x - prevPos.x != 0 || Pos.y - prevPos.y != 0) && curY == Pos.y) {
		
		int factor = animationFactor * charSpeed / speed;
		int timeFactor = int(factor * gameTime) % 4;

		int enemyRef = 4;
		if (index == 0)
			enemyRef = 40;

		TextureRef = enemyRef + timeFactor;

		if(index == 0)
			if (Audio::SFX[15 + going[2]].GetPlayStatus() == stopped) {
				Audio::SFX[16 - going[2]].PlayPause();
				going[2] = 1 - going[2];
			}
	}
}

void Enemy::IntoPit() {
	state = pitting;
	pitState = fallingToPit;
}

void Enemy::OutFromPit() {
	state = freeRun;
	pitState = out;
}
int Enemy::PathFinding() {

	//reminder for implementing the moving from the other document
	//base is untouched - ie. it is the same as the text file
	//golds are in .base 
	//enemies are in .act and not in base!
	//if a hole is created then the value of brick in act is emptied, but in the base it is unchanged!

	charSpeed = enemySpeed * speed;

	int x = int(Pos.x + 0.5);
	int y = int(Pos.y + 0.5);

	int runnerX = int(enemies[0].Pos.x + 0.5);
	int runnerY = int(enemies[0].Pos.y + 0.5);

	//while
	if (y == runnerY && enemies[0].state != falling) {
		while (x != runnerX) {
			layoutBlock checkable = layout[x][y];
			layoutBlock belowCheckable = layout[x][y - 1];

			int checkGold = Enemy::GoldChecker(x, y - 1);

			if (checkable == ladder || checkable == pole || belowCheckable == brick || belowCheckable == concrete || belowCheckable == ladder 
				|| EnemyChecker(x,y-1) || belowCheckable == pole || checkGold != -1 || holeTimer[x][y - 1] > 0) {
				
				//guard left to runner
				if (x < runnerX)
					++x;  					

				//guard right to runner
				else if (x > runnerX)
					--x;				 	
			}
			else
				break;						 // exit loop with closest x if no path to runner
		}

		//scan for a path ignoring walls is a success
		if (x == runnerX) {
			if (Pos.x < runnerX) {
				
				dPos.x = charSpeed;
				dPos.y = 0;
			}
			else if (Pos.x > runnerX) {
				
				dPos.x = -charSpeed;
				dPos.y = 0;
			}
			else {

				//this conditions are not necessary due to different implementations
				/*if (enemies[0].dPos.x < 0) 
					dPos.x = -charSpeed;
				
				else
					dPos.x = charSpeed;*/
			}
			return 1;
		}
	}

	ScanFloor();
}

void Enemy::ScanFloor() {

	int startX = int(Pos.x + 0.5);
	int startY = int(Pos.y + 0.5);

	int x = int(Pos.x + 0.5);
	int y = int(Pos.y + 0.5);

	bestPath = 0;
	bestRating = 255;

	while (x > 0) {                                    //get left end first
		layoutBlock checkable = layout[x - 1][y];
		layoutBlock belowCheckable = layout[x - 1][y - 1];

		if (checkable == brick || checkable == concrete)
			break;
		if (checkable == ladder || checkable == pole || belowCheckable == brick || belowCheckable == concrete || belowCheckable == ladder || holeTimer[x - 1][y - 1] > 0)
			--x;
		else {
			--x;                                        // go on left anyway 
			break;
		}
	}

	int leftEnd = x;

	x = startX;
	while (x < 29) {									// get right end next
		layoutBlock checkable = layout[x + 1][y];;
		layoutBlock belowCheckable = layout[x + 1][y - 1];

		if (checkable == brick || checkable == concrete)
			break;
		if (checkable == ladder || checkable == pole || belowCheckable == brick || belowCheckable == concrete || belowCheckable == ladder || holeTimer[x + 1][y - 1] > 0)
			++x;
		else {                                         // go on right anyway
			++x;
			break;
		}
	}

	int rightEnd = x;
	
	x = startX;
	layoutBlock checkable = layout[x][y];
	layoutBlock belowCheckable = layout[x][y - 1];

	//the possible directions (+0: stay), this numbers are the second arguments of the scandown and scanup functions (for the best path)
	//	1: left
	//	2: right
	//  3: up
	//  4: down

	if (belowCheckable != brick && belowCheckable != concrete && holeTimer[x][y - 1] <= 0)
		ScanDown(x, 4);
	if (checkable == ladder)
		ScanUp(x, 3);
	
	//first checking left direction, if curpath is 1 then we are checking the left side of the enemy, if 2, then the right side
	int curPath = 1;	
	x = leftEnd;

	while (true) {
		if (x == startX) {
			if (curPath == 1 && rightEnd != startX) {
				curPath = 2;
				x = rightEnd;
			}
			else
				break;
		}

		checkable = layout[x][y];
		belowCheckable = layout[x][y - 1];

		if (belowCheckable != brick && belowCheckable != concrete && holeTimer[x][y - 1] <= 0)
			ScanDown(x, curPath);

		if (checkable == ladder)
			ScanUp(x, curPath);

		if (curPath == 1)
			x++;
		else
			x--;
	}

	switch (bestPath) {
		case 1:
			dPos.x = -charSpeed;
			dPos.y = 0;
			break;

		case 2:
			dPos.x = charSpeed;
			dPos.y = 0;
			break;
		case 3:
			dPos.x = 0;
			dPos.y = charSpeed;
			break;

		case 4:
			dPos.x = 0;
			dPos.y = -charSpeed;
			break;

		default:
			dPos.x = 0;
			dPos.y = 0;
			break;
	}
}

void Enemy::ScanDown(int x, int curPath) {
	
	int y = int(Pos.y + 0.5);
	int runnerY = int(enemies[0].Pos.y + 0.5);

	layoutBlock belowCheckable = layout[x][y - 1];

	while (y > 0 && (belowCheckable = layout[x][y - 1]) != brick && belowCheckable != concrete && holeTimer[x][y - 1] <= 0) {
		
		int checkGold = Enemy::GoldChecker(x, y);

		//here is a problem, with champ 19?
		if (layout[x][y] != empty || checkGold != -1 || holeTimer[x][y] > 0) {
		
			layoutBlock leftSideBelow = layout[x - 1][y - 1];
			layoutBlock leftSide = layout[x - 1][y];

			layoutBlock rightSideBelow = layout[x + 1][y - 1];
			layoutBlock rightSide = layout[x + 1][y];

			if (x > 1) {                          
				if (leftSideBelow == brick || holeTimer[x - 1][y - 1] > 0 || leftSideBelow == concrete || leftSideBelow == ladder || leftSide == pole) 
					if (y <= runnerY)
						break; 
			}

			if (x < 28) {
				if (rightSideBelow == brick || holeTimer[x + 1][y - 1] > 0 || rightSideBelow == concrete ||	rightSideBelow == ladder ||	rightSide == pole)
					if (y <= runnerY)
						break;
			}
		}
		--y;
	}

	int curRating = 255;

	if (y == runnerY)
		curRating = abs(int(Pos.x + 0.5) - x);				// update best rating and direct.
	else if (y < runnerY)
		curRating = runnerY - y + 200;			// position below runner
	else
		curRating = y - runnerY + 100;			// position above runner

	if (curRating < bestRating) {

		bestRating = curRating;
		bestPath = curPath;
	}
}

void Enemy::ScanUp(int x, int curPath) {
	
	int y = int(Pos.y + 0.5);
	int runnerY = int(enemies[0].Pos.y + 0.5);

	while (y < 18 && layout[x][y] == ladder)	{
		++y;

		layoutBlock leftSideBelow = layout[x - 1][y - 1];
		layoutBlock leftSide = layout[x - 1][y];

		layoutBlock rightSideBelow = layout[x + 1][y - 1];
		layoutBlock rightSide = layout[x + 1][y];

		if (x > 1) {
			if (leftSideBelow == brick || holeTimer[x - 1][y - 1] > 0 || leftSideBelow == concrete || leftSideBelow == ladder || leftSide == pole) 
				if (y >= runnerY)
					break;           
		}

		if (x < 28)	{
			if (rightSideBelow == brick || holeTimer[x + 1][y - 1] > 0 || rightSideBelow == concrete ||	rightSideBelow == ladder ||	rightSide == pole)
				if (y >= runnerY)
					break;
		}
	}

	int curRating = 255;

	if (y == runnerY)
		curRating = abs(int(Pos.x +0.5) - x);			// update best rating and direct.
	else if (y < runnerY)
		curRating = runnerY - y + 200;		// position below runner   
	else
		curRating = y - runnerY + 100;		// position above runner    

	if (curRating < bestRating) {

		bestRating = curRating;
		bestPath = curPath;
	}
}