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
	brick = new std::unique_ptr<Brick>*[30];
	trapdoors = new std::unique_ptr<Trapdoor>*[30];

	for (int i = 0; i < 30; i++) {
		layout[i] = new LayoutBlock[18]{};
		brick[i] = new std::unique_ptr<Brick>[18];
		trapdoors[i] = new std::unique_ptr<Trapdoor>[18];
	}

	Enemy::setLayoutPointers(layout, brick, trapdoors, this);
	Brick::setLayoutPointers(layout, brick);
	Generator::setLayoutPointers(layout, brick, trapdoors, this);
}

void Play::start() {
	//player->SetIdleTime();
	GameTime::setSessionStartTime();

	if (Audio::sfx[4].getPlayStatus() == AudioStatus::playing) {
		Audio::sfx[4].playPause();
	}

	if (Audio::sfx[1].getPlayStatus() == AudioStatus::playing) {
		Audio::sfx[1].playPause();
	}

	if (Audio::sfx[17].getPlayStatus() == AudioStatus::playing) {
		Audio::sfx[17].playPause();
	}
}

void Play::update(float currentFrame) {
	GameTime::update(currentFrame);

	//play gamplay music cyclically
	if (Audio::sfx[7].getPlayStatus() != AudioStatus::playing) {
		Audio::sfx[7].playPause();
	}
	
	drawLevel();
	Enemy::handleCharacters();
	Gold::drawGolds();
	gamePlay->writeGameTime();

	//handle select and pause
	handleNonControlButtons();
}

void Play::end() {
	GameTime::setSessionEndTime();
}

void Play::handleNonControlButtons() {
	if (enter.simple()) {
		Audio::sfx[7].playPause();

		if (Audio::sfx[4].getPlayStatus() == AudioStatus::playing) {
			Audio::sfx[4].playPause();
		}

		if (Audio::sfx[1].getPlayStatus() == AudioStatus::playing) {
			Audio::sfx[1].playPause();
		}

		if (Audio::sfx[17].getPlayStatus() == AudioStatus::playing) {
			Audio::sfx[17].playPause();
		}

		Audio::sfx[14].playPause();

		gamePlay->transitionTo(gamePlay->pause);
	}

	//levelselect with space
	if (space.simple()) {
		Audio::sfx[17].stopAndRewind();
		Audio::sfx[4].stopAndRewind();
		Audio::sfx[7].stopAndRewind();

		if (gamePlay->stateContext->menuCursor < 2) {
			gamePlay->stateContext->transitionTo(gamePlay->stateContext->select);
		}
		else {
			gamePlay->stateContext->transitionTo(gamePlay->stateContext->generator);
		}
	}
}

void Play::clearContainers() {
	highestLadder = 0;

	Enemy::clearEnemyVector();
	Gold::clearGoldHolders();
	finishingLadders.clear();
}

void Play::setLadders(int highestLadder, std::vector<Vector2DInt> finishingLadders) {
	this->highestLadder = highestLadder;
	this->finishingLadders = finishingLadders;
}

void Play::drawLevel() {
	float gameTime = GameTime::getGameTime();
	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 18; j++) {
			if (brick[i][j]) {
				brick[i][j]->handle(gameTime);
			}
			//if layout is empty do nothing
			else if (layout[i][j] == LayoutBlock::empty);
			//animating flashing ladder
			else if (layout[i][j] == LayoutBlock::ladder) {
				Drawing::drawLevel(i, j, 12 + ladderFactor);
			}
			//draw every other item simply
			else if (layout[i][j] == LayoutBlock::brick) {
				Drawing::drawLevel(i, j, 0);
			}
			else if (layout[i][j] == LayoutBlock::trapDoor) {
				trapdoors[i][j]->handle();
			}
			else if (layout[i][j] == LayoutBlock::concrete) {
				Drawing::drawLevel(i, j, 6);
			}
			else if (layout[i][j] == LayoutBlock::pole) {
				Drawing::drawLevel(i, j, 18);
			}
		}
	}
}

void Play::loadLevel(unsigned int levelNumber) {
	clearContainers();

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


#ifndef ANDROID_VERSION
	std::fstream levelFile;
	levelFile.open(levelFileName);
	while (getline(levelFile, row)) {
#else
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
#endif
        if (foundLevel) {
			//filling last row with empty blocks
			if (rowCounter == 0) {
				for (int i = 1; i < 29; i++) {
					layout[i][17] = LayoutBlock::empty;
				}

				//but the sides are closed
				layout[0][17] = LayoutBlock::concrete;
				layout[29][17] = LayoutBlock::concrete;
				rowCounter++;
			}
			else {
				for (int i = 0; i < 30; i++) {
					brick[i][17 - rowCounter].reset();
					trapdoors[i][17 - rowCounter].reset();
					//level elements
					if (row[i] == '#') {
						Vector2DInt pos;
						pos.x = i;
						pos.y = 17 - rowCounter;

						layout[i][17 - rowCounter] = LayoutBlock::brick;
						brick[i][17 - rowCounter].reset(new Brick(pos));
					}
					else if (row[i] == '@' || row[i] == '"') {
						layout[i][17 - rowCounter] = LayoutBlock::concrete;
					}
					else if (row[i] == 'H') {
						layout[i][17 - rowCounter] = LayoutBlock::ladder;

						if (!highestLadder) {
							highestLadder = 17 - rowCounter;
						}

					}
					else if (row[i] == '-') {
						layout[i][17 - rowCounter] = LayoutBlock::pole;
					}
					else if (row[i] == 'X') {
						layout[i][17 - rowCounter] = LayoutBlock::trapDoor;
						trapdoors[i][17 - rowCounter].reset(new Trapdoor({ i,17-rowCounter}));
					}
					else if (row[i] == 'S') {
						layout[i][17 - rowCounter] = LayoutBlock::empty;
						finishingLadders.push_back({ i, 17 - rowCounter });
						if (!highestLadder) {
							highestLadder = 17 - rowCounter;
						}
					}
					else if (row[i] == '&') {							//runner
						layout[i][17 - rowCounter] = LayoutBlock::empty;

						Vector2DInt Pos = { i, 17 - rowCounter };
						Player::addPlayer(Pos);
					}

					else if (row[i] == '0') {							//guards
						layout[i][17 - rowCounter] = LayoutBlock::empty;

						Vector2DInt Pos = { i, 17 - rowCounter };
						Enemy::addEnemy(Pos);
					}

					else if (row[i] == '$') {							//gold
						layout[i][17 - rowCounter] = LayoutBlock::empty;
						Vector2DInt Pos = { i, 17 - rowCounter };

						std::unique_ptr<Gold> gold(new Gold(Pos));
						Gold::addGoldToUncollected(std::move(gold));
					}
					else {
						layout[i][17 - rowCounter] = LayoutBlock::empty;
					}
				}

				rowCounter++;

				//filling first row with concrete, then quit initializing level
				if (rowCounter == 17) {
					for (int i = 0; i < 30; i++) {
						layout[i][0] = LayoutBlock::concrete;
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
	if (gamePlay->stateContext->level[gamePlay->stateContext->playerNr] == 115 && gameVersion == 0) {
		highestLadder--;
	}
}

void Play::generateFinishingLadders() {
	for (auto finishLadder : finishingLadders) {
		layout[finishLadder.x][finishLadder.y] = LayoutBlock::ladder;
	}
}

short Play::getHighestLadder() {
	return highestLadder;
}

void Play::transitionToDeath() {
	gamePlay->transitionTo(gamePlay->death);
}

void Play::transitionToOutro(short killCounter, short goldNr, short fruitID) {
	gamePlay->stateContext->outro->setScoreParameters(killCounter, goldNr, fruitID);
	gamePlay->stateContext->transitionTo(gamePlay->stateContext->outro);
}