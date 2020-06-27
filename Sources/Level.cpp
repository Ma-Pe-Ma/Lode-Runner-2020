#include "Level.h"

void LevelLoading(unsigned int levelNumber) {
	std::fstream level;

	level.open(levelFileName);

	std::string row;

	if (levelNumber < 1)
		levelNumber = 1;
	if (levelNumber > 150)
		levelNumber = 150;

	std::string levelName = "Level 00" + std::to_string(levelNumber);

	if (levelNumber > 9)
		levelName = "Level 0" + std::to_string(levelNumber);

	if (levelNumber > 99)
		levelName = "Level " + std::to_string(levelNumber);

	Enemy::enemyNr = 1;

	bool foundLevel = false;
	//reading level into the layout matrix
	int rowCounter = 0;
	while (getline(level, row)) {
		if (foundLevel) {
			//filling last row with empty blocks
			if (rowCounter == 0) {
				for (int i = 1; i < 29; i++)
					layout[i][17] = empty;

				//but the sides are closed
				layout[0][17] = concrete;
				layout[29][17] = concrete;
				rowCounter++;
			}
			else {
				for (int i = 0; i < 30; i++) {

					holeTimer[i][17 - rowCounter] = 0;
					//level elements
					if (row[i] == '#') layout[i][17 - rowCounter] = brick;
					else if (row[i] == '@' || row[i] == '"')
						layout[i][17 - rowCounter] = concrete;
					else if (row[i] == 'H')
						layout[i][17 - rowCounter] = ladder;
					else if (row[i] == '-')
						layout[i][17 - rowCounter] = pole;
					else if (row[i] == 'X')
						layout[i][17 - rowCounter] = trapDoor;
					else if (row[i] == 'S') {
						layout[i][17 - rowCounter] = empty;
						holeTimer[i][17 - rowCounter] = -3.0f;
					}

					else if (row[i] == '&') {							//runner

						layout[i][17 - rowCounter] = empty;

						Vector2D Pos;
						Pos.x = i;
						Pos.y = 17 - rowCounter;

						Enemy::enemies[0].Initialize(0, Pos);
					}

					else if (row[i] == '0') {							//guards

						layout[i][17 - rowCounter] = empty;

						Vector2D Pos;
						Pos.x = i;
						Pos.y = 17 - rowCounter;

						Enemy::enemies[Enemy::enemyNr].Initialize(Enemy::enemyNr, Pos);
						Enemy::enemyNr++;
					}

					else if (row[i] == '$') {							//gold

						layout[i][17 - rowCounter] = empty;

						Vector2D Pos = { i, 17 - rowCounter };

						Gold::gold[Gold::goldNr].Initialize(Gold::goldNr, Pos);
						Gold::goldNr++;
					}
					else
						layout[i][17 - rowCounter] = empty;
				}

				rowCounter++;

				//filling first row with concrete, then quit initializing level
				if (rowCounter == 17) {
					for (int i = 0; i < 30; i++)
						layout[i][0] = concrete;
					foundLevel = false;
					break;
				}
			}
		}

		for (int i = 0; i < row.length(); i++)
			if (row.compare(i, levelName.length(), levelName) == 0)
				foundLevel = true;
	}

	for (int i = 0; i < 50; i++)
		Enemy::enemies[i].gold = Gold::gold;

	Gold::remainingGoldCount = Gold::goldNr;
	level.close();
}

void HighestLadder() {
	highestLadder = 0;

	for (int j = 0; j < 17; j++) {
		for (int i = 0; i < 30; i++) {

			if (holeTimer[i][j] == -3.0f) {
				layout[i][j] = ladder;
				holeTimer[i][j] = 0;
			}

			if (layout[i][j] == ladder) {
				highestLadder = j;
			}
		}
	}

	//my level ending conditions are different than the original, in most levels it's OK apart from this:
	//original conditions check if player is at the highest block or not
	if (level[playerNr] == 115 && !championship) {
		highestLadder--;
	}
}

void HighestLadderGen() {
	highestLadder = 0;

	for (int j = 0; j < 17; j++) {
		for (int i = 0; i < 30; i++) {
			
			if (gen[i][j] == 6) {
				layout[i][j] = ladder;
				holeTimer[i][j] = 0;
			}

			if (layout[i][j] == ladder) {
				highestLadder = j;
			}
		}
	}		
}