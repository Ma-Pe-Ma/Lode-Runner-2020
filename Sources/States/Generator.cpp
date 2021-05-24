#include "States/Generator.h"
#include "Audio.h"
#include "Drawing.h"
#include "Enemy.h"
#include "Gold.h"
#include "Player.h"
#include "States/StateContext.h"

LayoutBlock** Generator::layout;
std::unique_ptr <Brick>** Generator::brickO;
std::unique_ptr<Trapdoor>** Generator::trapdoors;
Play* Generator::play;

void Generator::setLayoutPointers(LayoutBlock** layout, std::unique_ptr <Brick>** brickO, std::unique_ptr <Trapdoor>** trapdoors, Play* play) {
	Generator::layout = layout;
	Generator::brickO = brickO;
	Generator::trapdoors = trapdoors;
	Generator::play = play;
}

Generator::Generator() {
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 17; j++) {
			if (j == 0) {
				gen[i][j] = 2;
			}
			else if (i == 0 || i == 29) {
				gen[i][j] = 2;
			}
			else {
				gen[i][j] = 0;
			}
		}
	}

	textureMap.insert(std::pair<short, short>(0, 17));
	textureMap.insert(std::pair<short, short>(1, 0));
	textureMap.insert(std::pair<short, short>(2, 6));
	textureMap.insert(std::pair<short, short>(3, 12));
	textureMap.insert(std::pair<short, short>(4, 18));
	textureMap.insert(std::pair<short, short>(5, 24));
	textureMap.insert(std::pair<short, short>(6, 30));
	textureMap.insert(std::pair<short, short>(7, 36));
	textureMap.insert(std::pair<short, short>(8, 42));
	textureMap.insert(std::pair<short, short>(9, 48));

	layoutMap.insert(std::pair<short, LayoutBlock> (0, empty));
	layoutMap.insert(std::pair<short, LayoutBlock> (1, brick));
	layoutMap.insert(std::pair<short, LayoutBlock> (2, concrete));
	layoutMap.insert(std::pair<short, LayoutBlock> (3, ladder));
	layoutMap.insert(std::pair<short, LayoutBlock> (4, pole));
	layoutMap.insert(std::pair<short, LayoutBlock> (5, trapDoor));
	layoutMap.insert(std::pair<short, LayoutBlock> (6, empty));
	layoutMap.insert(std::pair<short, LayoutBlock> (7, empty));
	layoutMap.insert(std::pair<short, LayoutBlock> (8, empty));
	layoutMap.insert(std::pair<short, LayoutBlock> (9, empty));
}

void Generator::start() {
	geX = 1;
	geY = 16;
	play->ClearContainers();
}

void Generator::update(float currentFrame) {
	if (rightButton.simple()) {
		if (++geX > 28) {
			geX = 28;
		}
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}
	}

	if (leftButton.simple()) {
		if (--geX < 1) {
			geX = 1;
		}
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}
	}

	if (up.simple()) {
		if (++geY > 16) {
			geY = 16;
		}
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}
	}

	if (down.simple()) {
		if (--geY < 1) {
			geY = 1;
		}
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}
	}

	//chaning element upwards
	if (rightDigButton.simple()) {
		if (++gen[geX][geY] > 9) {
			gen[geX][geY] = 0;
		}

		Audio::SFX[10].StopAndRewind();
		Audio::SFX[10].PlayPause();
	}

	//chaning element downwards
	if (leftDigButton.simple()) {
		if (--gen[geX][geY] < 0) {
			gen[geX][geY] = 9;
		}

		Audio::SFX[10].StopAndRewind();
		Audio::SFX[10].PlayPause();
	}

	//yellow cursor block
	if (int(3 * currentFrame) % 2) {
		DrawLevel(geX, geY, 54);
	}

	//drawing generator elements
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 17; j++) {
			int ref = textureMap[gen[i][j]];
			DrawLevel(i, j, ref);
		}
	}
	
	if (enter.simple()) {
		bool playerPresent = false;
		int highestLadder = 0;
		std::vector<Vector2DInt> finisihingLadders;

		for (int i = 0; i < 30; i++) {
			for (int j = 0; j < 17; j++) {
				brickO[i][j].reset();
				trapdoors[i][j].reset();

				Vector2DInt Pos = { i,j };
				if (gen[i][j] == 1) {
					brickO[i][j].reset(new Brick(Pos));
				}
				else if (gen[i][j] == 5) {
					trapdoors[i][j].reset(new Trapdoor(Pos));
				}
				else if (gen[i][j] == 6) {
					finisihingLadders.push_back(Pos);
				}
				else if (gen[i][j] == 7) {
					std::unique_ptr<Gold> gold(new Gold(Pos));
					Gold::addGoldToUncollected(std::move(gold));
				}
				else if (gen[i][j] == 8) {
					Enemy::AddEnemy(Pos);
				}
				else if (gen[i][j] == 9) {
					if (!playerPresent) {
						Player::AddPlayer(Pos);
						playerPresent = true;
					}
				}

				layout[i][j] = layoutMap[gen[i][j]];
			}
		}

		//if no player was given, put in one!
		if (!playerPresent) {
			for (int j = 1; j < 18; j++) {
				if (playerPresent) {
					break;
				}

				for (int i = 1; i < 30; i++) {
					if (layout[i][j] == empty && gen[i][j] != 7 && gen[i][j] != 8) {
						Vector2DInt Pos = { i,j };
						Player::AddPlayer(Pos);
						playerPresent = true;
						break; 
					}
				}
			}
		}

		if (playerPresent) {
			highestLadder = highestLadder < 15 ? 15 : highestLadder;

			play->SetLadders(highestLadder, finisihingLadders);
			stateContext->TransitionTo(stateContext->gamePlay);
		}		
	}
}

void Generator::end() {

}