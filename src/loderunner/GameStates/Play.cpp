#include "GameStates/Play.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Enemy.h"
#include "Player.h"
#include "Gold.h"
#include "Drawing.h"

//#include "Generator.h"

#include "GameTime.h"
#include <Shader.h>

#include <array>

Play::Play() {
	layout = new LayoutBlock*[30];
	brick = new std::shared_ptr<Brick>*[30];
	trapdoors = new std::shared_ptr<Trapdoor>*[30];

	for (int i = 0; i < 30; i++) {
		layout[i] = new LayoutBlock[18]{};
		brick[i] = new std::shared_ptr<Brick>[18];
		trapdoors[i] = new std::shared_ptr<Trapdoor>[18];
	}

	Enemy::setLayoutPointers(layout, brick, trapdoors, this);
	Brick::setLayoutPointers(layout, brick);

	brickList = std::make_shared<std::vector<std::shared_ptr<Brick>>>();
	
#ifdef NDEBUG
	Generator::setLayoutPointers(layout, brick, trapdoors, this);
#endif
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

	//play gameplay music cyclically
	if (Audio::sfx[7].getPlayStatus() != AudioStatus::playing) {
		Audio::sfx[7].playPause();
	}
	
	Enemy::handleCharacters();
	
	float gameTime = GameTime::getGameTime();
	for (auto brick : *brickList)
	{
		brick->handle(gameTime);
	}

	drawLevel();
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

inline void Play::drawLevel() {
	float gameTime = GameTime::getGameTime();
	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;
	renderingManager.setLadderFlashFactor(ladderFactor);
	renderingManager.render();
}

void Play::loadLevel(unsigned int levelNumber) {
	clearContainers();
	brickList->clear();

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;
	std::vector<std::tuple<int, int>> finishingLadderList;

	std::vector<std::shared_ptr<Trapdoor>> trapDoorList;

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
				concreteList.push_back({ 0, 17 });
				concreteList.push_back({ 29, 17 });
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

						std::shared_ptr<Brick> newBrick = std::make_shared<Brick>(pos);

						brickList->push_back(newBrick);
						brick[i][17 - rowCounter] = newBrick;
					}
					else if (row[i] == '@' || row[i] == '"') {
						layout[i][17 - rowCounter] = LayoutBlock::concrete;
						concreteList.push_back({ i, 17 - rowCounter });
					}
					else if (row[i] == 'H') {
						layout[i][17 - rowCounter] = LayoutBlock::ladder;
						ladderList.push_back({ i, 17 - rowCounter });

						if (!highestLadder) {
							highestLadder = 17 - rowCounter;
						}

					}
					else if (row[i] == '-') {
						layout[i][17 - rowCounter] = LayoutBlock::pole;
						poleList.push_back({i, 17 - rowCounter});
					}
					else if (row[i] == 'X') {
						layout[i][17 - rowCounter] = LayoutBlock::trapDoor;

						std::shared_ptr<Trapdoor> trapdoor = std::make_shared<Trapdoor>(Trapdoor({ i, 17 - rowCounter }));
					
						trapDoorList.push_back(trapdoor);
						trapdoors[i][17 - rowCounter] = trapdoor;
					}
					else if (row[i] == 'S') {
						layout[i][17 - rowCounter] = LayoutBlock::empty;
						finishingLadders.push_back({ i, 17 - rowCounter });
						finishingLadderList.push_back({ i, 17 - rowCounter });
						if (!highestLadder) {
							highestLadder = 17 - rowCounter;
						}
					}
					else if (row[i] == '&') {							//runner
						layout[i][17 - rowCounter] = LayoutBlock::empty;

						Vector2DInt pos = { i, 17 - rowCounter };
						Player::addPlayer(pos);
					}

					else if (row[i] == '0') {							//guards
						layout[i][17 - rowCounter] = LayoutBlock::empty;

						Vector2DInt pos = { i, 17 - rowCounter };
						Enemy::addEnemy(pos);
					}

					else if (row[i] == '$') {							//gold
						layout[i][17 - rowCounter] = LayoutBlock::empty;
						Vector2DInt pos = { i, 17 - rowCounter };

						std::unique_ptr<Gold> gold(new Gold(pos));
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
						concreteList.push_back({ i, 0 });
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

	renderingManager.clearRenderableObjects();
	renderingManager.setPoleList(poleList);
	renderingManager.setConcreteList(concreteList);
	renderingManager.setBrickList(brickList);
	renderingManager.setTrapdoorList(trapDoorList);
	renderingManager.setLadderList(ladderList);
	renderingManager.setFinishingLadderList(finishingLadderList);
	renderingManager.initialize();
}

void Play::generateFinishingLadders() {
	renderingManager.enableFinishingLadderDrawing();

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