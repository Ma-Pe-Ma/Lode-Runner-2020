#include "GameStates/Play.h"
#include "States/GamePlay.h"
#include "States/StateContext.h"

#include "Enemy.h"
#include "Player.h"
#include "Gold.h"

//#include "Generator.h"
#include "GameTime.h"
#include "Audio/AudioFile.h"

void Play::start() {
	//player->SetIdleTime();
	GameTime::setSessionStartTime();

	if (gameContext->getAudio()->getAudioFileByID(4)->getPlayStatus() == AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(4)->playPause();
	}

	if (gameContext->getAudio()->getAudioFileByID(1)->getPlayStatus() == AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(1)->playPause();
	}

	if (gameContext->getAudio()->getAudioFileByID(17)->getPlayStatus() == AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(17)->playPause();
	}
}

void Play::update(float currentFrame) {
	GameTime::update(currentFrame);

	//play gameplay music cyclically
	if (gameContext->getAudio()->getAudioFileByID(7)->getPlayStatus() != AudioStatus::playing) {
		gameContext->getAudio()->getAudioFileByID(7)->playPause();
	}

	gameContext->run();	

	std::string timeValue = std::to_string(GameTime::getGameTime());
	timeValue = timeValue.substr(0, timeValue.length() - 5);
	timeText->changeContent("GAMETIME: " + timeValue + " SEC");

	drawScene();

	//handle select and pause
	handleNonControlButtons();
}

void Play::end() {
	GameTime::setSessionEndTime();
}

void Play::handleNonControlButtons() {
	if (gameContext->getIOContext()->getEnterButton().simple()) {
		gameContext->getAudio()->getAudioFileByID(7)->playPause();

		if (gameContext->getAudio()->getAudioFileByID(4)->getPlayStatus() == AudioStatus::playing) {
			gameContext->getAudio()->getAudioFileByID(4)->playPause();
		}

		if (gameContext->getAudio()->getAudioFileByID(1)->getPlayStatus() == AudioStatus::playing) {
			gameContext->getAudio()->getAudioFileByID(1)->playPause();
		}

		if (gameContext->getAudio()->getAudioFileByID(17)->getPlayStatus() == AudioStatus::playing) {
			gameContext->getAudio()->getAudioFileByID(17)->playPause();
		}

		gameContext->getAudio()->getAudioFileByID(14)->playPause();

		gamePlay->transitionToAtEndOfFrame(gamePlay->getPause());
	}

	//levelselect with space
	if (gameContext->getIOContext()->getSpaceButton().simple()) {
		gameContext->getAudio()->getAudioFileByID(17)->stopAndRewind();
		gameContext->getAudio()->getAudioFileByID(4)->stopAndRewind();
		gameContext->getAudio()->getAudioFileByID(7)->stopAndRewind();

		if (gamePlay->getStateContext()->menuCursor < 2) {
			gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getSelect());
		}
		else {
			gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getGenerator());
		}
	}
}

void Play::clearContainers() {
	gameContext->clearContainers();
}

void Play::drawScene() {
	float gameTime = GameTime::getGameTime();
	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;
	gameContext->getRenderingManager()->setLadderFlashFactor(ladderFactor);
	gameContext->getRenderingManager()->render();
}

void Play::transitionToDeath() {
	gamePlay->transitionToAtEndOfFrame(gamePlay->getDeath());
}

void Play::transitionToOutro(short killCounter, short goldNr, short fruitID) {
	gamePlay->getStateContext()->getOutro()->setScoreParameters(killCounter, goldNr, fruitID);
	gamePlay->getStateContext()->transitionToAtEndOfFrame(gamePlay->getStateContext()->getOutro());
}

void Play::loadLevel(unsigned int levelNumber) {
	clearContainers();

	short highestLadder = 0;

	LayoutBlock** layout = new LayoutBlock * [30];
	std::shared_ptr<Brick>** bricks = new std::shared_ptr<Brick>*[30];
	std::shared_ptr<Trapdoor>** trapdoors = new std::shared_ptr<Trapdoor>*[30];

	for (int i = 0; i < 30; i++) {
		layout[i] = new LayoutBlock[18]{};
		bricks[i] = new std::shared_ptr<Brick>[18];
		trapdoors[i] = new std::shared_ptr<Trapdoor>[18];

		//bricks[i] = new Brick*[18];
		//trapdoors[i] = new Trapdoor*[18];		

		//for (int j = 0; j < 18; j++)
		//{
			//bricks[i][j] = nullptr;
		//}
	}

	std::vector<std::shared_ptr<Brick>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapDoorList;
	std::vector<std::shared_ptr<Gold>> goldList;
	std::vector<std::shared_ptr<Enemy>> enemyList;

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;
	std::vector<std::tuple<int, int>> finishingLadderList;

	std::shared_ptr<Player> player = nullptr;

	levelNumber = levelNumber < 1 ? 1 : levelNumber;
	levelNumber = levelNumber > 150 ? 150 : levelNumber;

	std::string levelName = std::to_string(levelNumber);
	levelName = "Level " + levelName.insert(0, 3 - levelName.length(), '0');

	std::string row;
	bool foundLevel = false;
	//reading level into the layout matrix
	int rowCounter = 0;

	std::shared_ptr<IOContext> ioContext = gamePlay->getStateContext()->getIOContext();
	const std::string levelFileName = gameContext->getGameConfiguration()->getLevelFileName();

	ioContext->loadLevel(levelFileName, [&](std::string row) -> bool {
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
					//level elements
					if (row[i] == '#') {
						Vector2DInt pos;
						pos.x = i;
						pos.y = 17 - rowCounter;

						layout[i][17 - rowCounter] = LayoutBlock::brick;

						std::shared_ptr<Brick> newBrick = std::make_shared<Brick>(pos);
						newBrick->setGameContext(gameContext);

						brickList.push_back(newBrick);
						bricks[i][17 - rowCounter] = newBrick;
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
						poleList.push_back({ i, 17 - rowCounter });
					}
					else if (row[i] == 'X') {
						layout[i][17 - rowCounter] = LayoutBlock::trapDoor;

						std::shared_ptr<Trapdoor> trapdoor = std::make_shared<Trapdoor>(Trapdoor({ i, 17 - rowCounter }));

						trapDoorList.push_back(trapdoor);
						trapdoors[i][17 - rowCounter] = trapdoor;
					}
					else if (row[i] == 'S') {
						layout[i][17 - rowCounter] = LayoutBlock::empty;
						finishingLadderList.push_back({ i, 17 - rowCounter });
						if (!highestLadder) {
							highestLadder = 17 - rowCounter;
						}
					}
					else if (row[i] == '&') {							//runner
						layout[i][17 - rowCounter] = LayoutBlock::empty;

						player = std::make_shared<Player>(float(i), float(17 - rowCounter));
						player->setGameContext(gameContext);
						player->setCharSpeed(gameContext->getGameConfiguration()->getPlayerSpeed());
						player->setIOContext(gameContext->getIOContext());
					}

					else if (row[i] == '0') {							//guards
						layout[i][17 - rowCounter] = LayoutBlock::empty;

						std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(float(i), float(17 - rowCounter));
						enemy->setGameContext(gameContext);
						enemy->setCharSpeed(gameContext->getGameConfiguration()->getEnemySpeed());

						enemyList.push_back(enemy);
					}

					else if (row[i] == '$') {							//gold
						layout[i][17 - rowCounter] = LayoutBlock::empty;
						Vector2DInt pos = { i, 17 - rowCounter };

						std::shared_ptr<Gold> gold = std::make_shared<Gold>(pos);
						goldList.push_back(gold);
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

					//stop the reading process as the level is loaded
					return true;
				}
			}
		}

		for (int i = 0; i < row.length(); i++) {
			if (row.compare(i, levelName.length(), levelName) == 0) {
				foundLevel = true;
			}
		}

		return false;
	});

	//my level ending conditions are different than the original, in most levels it's OK apart from this:
	//original conditions check if player is at the highest block or not
	if (gamePlay->getStateContext()->level[gamePlay->getStateContext()->playerNr] == 115 && gameContext->getGameConfiguration()->getGameVersion() == 0) {
		highestLadder--;
	}

	timeText = std::make_shared<Text>(Text("GAMETIME: 0.0 SEC   ", { -5, 0 }));

	std::vector<std::shared_ptr<Text>> textList;
	textList.push_back(timeText);

	enemyList.insert(enemyList.end(), player);

	gameContext->setHighestLadder(highestLadder);

	gameContext->getRenderingManager()->clearRenderableObjects();
	gameContext->getRenderingManager()->setPoleList(poleList);
	gameContext->getRenderingManager()->setConcreteList(concreteList);
	gameContext->getRenderingManager()->setBrickList(brickList);
	gameContext->getRenderingManager()->setTrapdoorList(trapDoorList);
	gameContext->getRenderingManager()->setLadderList(ladderList);
	gameContext->getRenderingManager()->setFinishingLadderList(finishingLadderList);
	gameContext->getRenderingManager()->setGoldList(goldList);
	gameContext->getRenderingManager()->initializeLevelLayout();

	gameContext->getRenderingManager()->setEnemyList(enemyList);
	gameContext->getRenderingManager()->initializeEnemies();

	gameContext->getRenderingManager()->setTextList(textList);
	gameContext->getRenderingManager()->initializeCharacters();

	enemyList.erase(enemyList.end() - 1);
	gameContext->setEnemies(enemyList);
	gameContext->setPlayer(player);
	gameContext->setBrickList(brickList);
	gameContext->setTrapdoorList(trapDoorList);
	gameContext->setTrapdoors(trapdoors);
	gameContext->setLayout(layout);
	gameContext->setHighestLadder(highestLadder);
	gameContext->setFinishingLadders(finishingLadderList);
	gameContext->setBricks(bricks);
	gameContext->setUncollectedGoldList(goldList);
}