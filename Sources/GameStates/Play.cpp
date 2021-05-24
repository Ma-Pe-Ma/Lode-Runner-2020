#include "GameStates/Play.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Enemy.h"
#include "Player.h"
#include "Gold.h"
#include "Drawing.h"

//#include "Generator.h"

#include "GameTime.h"

Play::Play() {
	layout = new LayoutBlock*[30];
	brickO = new std::unique_ptr<Brick>*[30];
	trapdoors = new std::unique_ptr<Trapdoor>*[30];

	for (int i = 0; i < 30; i++) {
		layout[i] = new LayoutBlock[18]{};
		brickO[i] = new std::unique_ptr<Brick>[18];
		trapdoors[i] = new std::unique_ptr<Trapdoor>[18];
	}

	Enemy::setLayoutPointers(layout, brickO, trapdoors, this);
	Brick::setLayoutPointers(layout, brickO);
	Generator::setLayoutPointers(layout, brickO, trapdoors, this);
}

void Play::start() {
	//player->SetIdleTime();
	GameTime::setSessionStartTime();

	if (Audio::SFX[4].GetPlayStatus() == playing) {
		Audio::SFX[4].PlayPause();
	}

	if (Audio::SFX[1].GetPlayStatus() == playing) {
		Audio::SFX[1].PlayPause();
	}

	if (Audio::SFX[17].GetPlayStatus() == playing) {
		Audio::SFX[17].PlayPause();
	}
}

void Play::update(float currentFrame) {
	GameTime::update(currentFrame);

	//play gamplay music cyclically
	if (Audio::SFX[7].GetPlayStatus() != playing) {
		Audio::SFX[7].PlayPause();
	}
	
	drawLevel();
	Enemy::handleCharacters();
	Gold::DrawGolds();
	gamePlay->WriteGameTime();

	//handle select and pause
	handleNonControlButtons();
}

void Play::end() {
	GameTime::setSessionEndTime();
}

void Play::handleNonControlButtons() {
	if (enter.simple()) {
		Audio::SFX[7].PlayPause();

		if (Audio::SFX[4].GetPlayStatus() == playing) {
			Audio::SFX[4].PlayPause();
		}

		if (Audio::SFX[1].GetPlayStatus() == playing) {
			Audio::SFX[1].PlayPause();
		}

		if (Audio::SFX[17].GetPlayStatus() == playing) {
			Audio::SFX[17].PlayPause();
		}

		Audio::SFX[14].PlayPause();

		gamePlay->TransitionTo(gamePlay->pause);
	}

	//levelselect with space
	if (space.simple()) {
		Audio::SFX[17].StopAndRewind();
		Audio::SFX[4].StopAndRewind();
		Audio::SFX[7].StopAndRewind();

		if (gamePlay->stateContext->menuCursor < 2) {
			gamePlay->stateContext->TransitionTo(gamePlay->stateContext->select);
		}
		else {
			gamePlay->stateContext->TransitionTo(gamePlay->stateContext->generator);
		}
	}
}

void Play::ClearContainers() {
	highestLadder = 0;

	Enemy::clearEnemyVector();
	Gold::ClearGoldHolders();
	finishingLadders.clear();
}

void Play::SetLadders(int highestLadder, std::vector<Vector2DInt> finishingLadders) {
	this->highestLadder = highestLadder;
	this->finishingLadders = finishingLadders;
}

void Play::drawLevel() {
	float gameTime = GameTime::getGameTime();
	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 18; j++) {
			if (brickO[i][j]) {
				brickO[i][j]->Handle(gameTime);
			}
			//if layout is empty do nothing
			else if (layout[i][j] == empty);
			//animating flashing ladder
			else if (layout[i][j] == ladder) {
				DrawLevel(i, j, 12 + ladderFactor);
			}
			//draw every other item simply
			else if (layout[i][j] == brick) {
				DrawLevel(i, j, 0);
			}
			else if (layout[i][j] == trapDoor) {
				trapdoors[i][j]->handle();
			}
			else if (layout[i][j] == concrete) {
				DrawLevel(i, j, 6);
			}
			else if (layout[i][j] == pole) {
				DrawLevel(i, j, 18);
			}
		}
	}
}

void Play::loadLevel(unsigned int levelNumber) {
	ClearContainers();

	levelNumber = levelNumber < 1 ? 1 : levelNumber;
	levelNumber = levelNumber > 150 ? 150 : levelNumber;

	std::string levelName = "";
	if (levelNumber / 100 != 0) {
		levelName = "Level " + std::to_string(levelNumber);
	}
	else {
		if (levelNumber / 10 != 0) {
			levelName = "Level 0" + std::to_string(levelNumber);
		}
		else {
			levelName = "Level 00" + std::to_string(levelNumber);
		}
	}

	std::string row;
	bool foundLevel = false;
	//reading level into the layout matrix
	int rowCounter = 0;

#ifdef ANDROID_VERSION
	std::vector<uint8_t> data;
	if (!ndk_helper::JNIHelper::GetInstance()->ReadFile(levelFileName.c_str(), &data)) {
		Helper::log("Can't read file!"+levelFileName);
		LOGI("Can not open a file:%s", levelFileName.c_str());
		return;
	}

	const GLchar *source = (GLchar *)&data[0];
	std::string test1(source);

	std::stringstream test;
	test<<test1;
	std::string segment;
	std::vector<std::string> rows;

	while(std::getline(test, segment, '\n')) {
		rows.push_back(segment);
	}

	for (auto row : rows) {
#else
	std::fstream levelFile;
	levelFile.open(levelFileName);
	while (getline(levelFile, row)) {
#endif
        if (foundLevel) {
			//filling last row with empty blocks
			if (rowCounter == 0) {
				for (int i = 1; i < 29; i++) {
					layout[i][17] = empty;
				}

				//but the sides are closed
				layout[0][17] = concrete;
				layout[29][17] = concrete;
				rowCounter++;
			}
			else {
				for (int i = 0; i < 30; i++) {
					brickO[i][17 - rowCounter].reset();
					trapdoors[i][17 - rowCounter].reset();
					//level elements
					if (row[i] == '#') {
						Vector2DInt Pos;
						Pos.x = i;
						Pos.y = 17 - rowCounter;

						layout[i][17 - rowCounter] = brick;
						brickO[i][17 - rowCounter].reset(new Brick(Pos));
					}
					else if (row[i] == '@' || row[i] == '"') {
						layout[i][17 - rowCounter] = concrete;
					}
					else if (row[i] == 'H') {
						layout[i][17 - rowCounter] = ladder;

						if (!highestLadder) {
							highestLadder = 17 - rowCounter;
						}

					}
					else if (row[i] == '-') {
						layout[i][17 - rowCounter] = pole;
					}
					else if (row[i] == 'X') {
						layout[i][17 - rowCounter] = trapDoor;
						trapdoors[i][17 - rowCounter].reset(new Trapdoor({ i,17-rowCounter}));
					}
					else if (row[i] == 'S') {
						layout[i][17 - rowCounter] = empty;
						finishingLadders.push_back({ i, 17 - rowCounter });
						if (!highestLadder) {
							highestLadder = 17 - rowCounter;
						}
					}
					else if (row[i] == '&') {							//runner
						layout[i][17 - rowCounter] = empty;

						Vector2DInt Pos = { i, 17 - rowCounter };
						Player::AddPlayer(Pos);
					}

					else if (row[i] == '0') {							//guards
						layout[i][17 - rowCounter] = empty;

						Vector2DInt Pos = { i, 17 - rowCounter };
						Enemy::AddEnemy(Pos);
					}

					else if (row[i] == '$') {							//gold
						layout[i][17 - rowCounter] = empty;
						Vector2DInt Pos = { i, 17 - rowCounter };

						std::unique_ptr<Gold> gold(new Gold(Pos));
						Gold::addGoldToUncollected(std::move(gold));
					}
					else {
						layout[i][17 - rowCounter] = empty;
					}
				}

				rowCounter++;

				//filling first row with concrete, then quit initializing level
				if (rowCounter == 17) {
					for (int i = 0; i < 30; i++) {
						layout[i][0] = concrete;
					}

					foundLevel = false;
					break;
				}
			}
		}

		for (int i = 0; i < row.length(); i++) {
			if (row.compare(i, levelName.length(), levelName) == 0) {
				foundLevel = true;
			}
		}
	}

#ifndef ANDROID_VERSION
	levelFile.close();
#endif
	//my level ending conditions are different than the original, in most levels it's OK apart from this:
	//original conditions check if player is at the highest block or not
	if (gamePlay->stateContext->level[gamePlay->stateContext->playerNr] == 115 && !championship) {
		highestLadder--;
	}
}

void Play::generateFinishingLadders() {
	for (auto finishLadder : finishingLadders) {
		layout[finishLadder.x][finishLadder.y] = ladder;
	}
}

short Play::getHighestLadder() {
	return highestLadder;
}

void Play::TransitionToDeath() {
	gamePlay->TransitionTo(gamePlay->death);
}

void Play::TransitionToOutro(short killCounter, short goldNr, short fruitID) {
	gamePlay->stateContext->outro->SetScoreParameters(killCounter, goldNr, fruitID);
	gamePlay->stateContext->TransitionTo(gamePlay->stateContext->outro);
}