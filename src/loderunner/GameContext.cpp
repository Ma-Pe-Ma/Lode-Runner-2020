#include "GameContext.h"
#include "Rendering/RenderingManager.h"

#include "Player.h"
#include "Gold.h"

#include "GameStates/Play.h"

void GameContext::run() {
	float gameTime = calculateEllapsedTime();
	float frameDelta = calculateFrameDelta();

	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;
	getRenderingManager()->setLadderFlashFactor(ladderFactor);

	std::string timeValue = std::to_string(gameTime);
	timeValue = timeValue.substr(0, timeValue.length() - 5);
	timeText->changeContent("GAMETIME: " + timeValue + " SEC");

#ifndef RELEASE_VERSION
	//enemies[0]->setDPos(ioContext->debugPos[0]);
	//enemies[1]->setDPos(ioContext->debugPos[1]);
#endif // !RELEASE_VERSION

	player->handle(gameTime, frameDelta);

	for (auto& enemy : enemies) {
		enemy->handle(gameTime, frameDelta);
	}

#ifndef RELEASE_VERSION
	//ioContext->debugPos[0] = { 0.0f, 0.0f };
	//ioContext->debugPos[1] = { 0.0f, 0.0f };
#endif

	for (auto brick : brickList)
	{
		brick->handle(gameTime);
	}
}

bool GameContext::checkDigPrevention(int x, int y) {
	for (auto& enemy : enemies) {
		if (std::abs(enemy->getPos().x - x) <= 0.75f && y + 1 <= enemy->getPos().y && enemy->getPos().y < y + 1.5f) {
			return true;
		}
	}

	return false;
}

void GameContext::notifyPlayerAboutDigEnd() {
	player->releaseFromDigging();
}

void GameContext::checkDeaths(int x, int y)
{
	player->checkDeath(x, y);

	for (auto& enemy : enemies) {
		enemy->checkDeath(x, y);
	}
}

void GameContext::checkPlayerDeathByEnemy()
{
	//check if runner dies by enemy
	for (auto& enemy : enemies) {
		if (std::abs(enemy->getPos().x - player->getPos().x) < 0.5f && std::abs(enemy->getPos().y - player->getPos().y) < 0.5f) {
			player->die();
		}
	}
}

void GameContext::handlePlayerDying() {
	player->dying();
}

bool GameContext::enemyCarriesGold()
{
	for (auto& enemy : enemies) {
		if (enemy->carriesGold()) {
			return true;
		}
	}

	return false;
}

std::shared_ptr<Gold> GameContext::goldCollectChecker(float x, float y) {
	for (auto it = uncollectedGoldList.begin(); it != uncollectedGoldList.end(); it++) {
		if (std::abs(it->get()->getPos().x - x) < 0.15f && std::abs(it->get()->getPos().y - y) < 0.15f) {
			std::shared_ptr<Gold> foundGold = *it;
			uncollectedGoldList.erase(it);

			return foundGold;
		}
	}

	return nullptr;
}

bool GameContext::goldChecker(int x, int y) {
	for (auto& gold : uncollectedGoldList) {
		if (gold->getPos().x == x && gold->getPos().y == y) {
			return true;
		}
	}

	return false;
}

void GameContext::addGoldToCollectedList(std::shared_ptr<Gold> collectedGold) {
	collectedGold->setPos(Vector2DInt{ -1, -1 });
	this->collectedGoldList.push_back(collectedGold);
}

void GameContext::addGoldToUncollectedList(std::shared_ptr<Gold> newGold) {
	this->uncollectedGoldList.push_back(newGold);
}

void GameContext::generateFinishingLadders() {
	renderingManager->enableFinishingLadderDrawing();

	for (auto finishLadder : finishingLadders) {
		int x = std::get<0>(finishLadder);
		int y = std::get<1>(finishLadder);

		layout[x][y] = LayoutBlock::ladder;
	}

	finishingLadders.clear();
}

void GameContext::transitionToDeath()
{
	play->transitionToDeath();
}

void GameContext::transitionToOutro()
{
	play->transitionToOutro(killCounter, getCollectedGoldSize(), 0);
}

void GameContext::clearContainers()
{
	highestLadder = 0;
	killCounter = 0;
	
	player = nullptr;
	enemies.clear();
	finishingLadders.clear();	
	uncollectedGoldList.clear();
	collectedGoldList.clear();

	brickList.clear();
	trapdoorList.clear();

	bricks = nullptr;
	trapdoors = nullptr;

	timeText = nullptr;
}

void GameContext::loadLevel(unsigned int levelNumber)
{
	clearContainers();
	
	bricks = std::make_shared<std::shared_ptr<std::shared_ptr<Brick>[]>[]>(30);
	trapdoors = std::make_shared<std::shared_ptr<std::shared_ptr<Trapdoor>[]>[]>(30);;

	for (int i = 0; i < 30; i++) {		
		bricks[i] = std::make_shared<std::shared_ptr<Brick>[]>(18);
		trapdoors[i] = std::make_shared<std::shared_ptr<Trapdoor>[]>(18);

		for (int j = 0; j< 18; j++)
		{
			layout[i][j] = LayoutBlock::empty;
		}
	}

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;

	levelNumber = levelNumber < 1 ? 1 : levelNumber;
	levelNumber = levelNumber > 150 ? 150 : levelNumber;

	std::string levelName = std::to_string(levelNumber);
	levelName = "Level " + levelName.insert(0, 3 - levelName.length(), '0');

	std::string row;
	bool foundLevel = false;
	//reading level into the layout matrix
	int rowCounter = 0;

	std::shared_ptr<IOContext> ioContext = getIOContext();
	const std::string levelFileName = getGameConfiguration()->getLevelFileName();

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
						newBrick->setGameContext(this);

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

						trapdoorList.push_back(trapdoor);
						trapdoors[i][17 - rowCounter] = trapdoor;
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

						player = std::make_shared<Player>(float(i), float(17 - rowCounter));
						player->setGameContext(this);
						player->setCharSpeed(gameConfiguration->getPlayerSpeed());
						player->setIOContext(getIOContext());
					}

					else if (row[i] == '0') {							//guards
						layout[i][17 - rowCounter] = LayoutBlock::empty;

						std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(float(i), float(17 - rowCounter));
						enemy->setGameContext(this);
						enemy->setCharSpeed(gameConfiguration->getEnemySpeed());

						enemies.push_back(enemy);
					}

					else if (row[i] == '$') {							//gold
						layout[i][17 - rowCounter] = LayoutBlock::empty;
						Vector2DInt pos = { i, 17 - rowCounter };

						std::shared_ptr<Gold> gold = std::make_shared<Gold>(pos);
						uncollectedGoldList.push_back(gold);
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
	if (levelNumber == 115 && gameConfiguration->getGameVersion() == 0) {
		highestLadder--;
	}

	timeText = std::make_shared<Text>(Text("GAMETIME: 0.0 SEC   ", { -5, 0 }));
	std::vector<std::shared_ptr<Text>> textList;
	textList.push_back(timeText);

	enemies.insert(enemies.end(), player);

	renderingManager->clearRenderableObjects();
	renderingManager->setPoleList(poleList);
	renderingManager->setConcreteList(concreteList);
	renderingManager->setBrickList(brickList);
	renderingManager->setTrapdoorList(trapdoorList);
	renderingManager->setLadderList(ladderList);
	renderingManager->setFinishingLadderList(finishingLadders);
	renderingManager->setGoldList(uncollectedGoldList);
	renderingManager->initializeLevelLayout();

	renderingManager->setEnemyList(enemies);
	renderingManager->initializeEnemies();

	renderingManager->setTextList(textList);
	renderingManager->initializeCharacters();

	enemies.erase(enemies.end() - 1);

#ifndef NDEBUG
	// Set the first two enemy to controllable when debugging
	for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); enemyIt++)
	{
		int index = enemyIt - enemies.begin();

		if (index < 2) {
			(*enemyIt)->setDebugEnemyState(index + 1);
		}
		else {
			break;
		}
	}
#endif
}

void GameContext::generateLevel(short gen[30][18])
{
	clearContainers();

	bricks = std::make_shared<std::shared_ptr<std::shared_ptr<Brick>[]>[]>(30);
	trapdoors = std::make_shared<std::shared_ptr<std::shared_ptr<Trapdoor>[]>[]>(30);;

	for (int i = 0; i < 30; i++) {
		bricks[i] = std::make_shared<std::shared_ptr<Brick>[]>(18);
		trapdoors[i] = std::make_shared<std::shared_ptr<Trapdoor>[]>(18);

		for (int j = 0; j < 18; j++)
		{
			layout[i][j] = LayoutBlock::empty;
		}
	}

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 18; j++) {
			Vector2DInt pos = { i, j };

			if (gen[i][j] == 1) {
				std::shared_ptr<Brick> newBrick = std::make_shared<Brick>(Brick({ pos.x, pos.y }));

				brickList.push_back(newBrick);
				bricks[i][j] = newBrick;

				newBrick->setGameContext(this);
			}
			else if (gen[i][j] == 2) {
				concreteList.push_back({ pos.x, pos.y });
			}
			else if (gen[i][j] == 3) {
				ladderList.push_back({ pos.x, pos.y });
			}
			else if (gen[i][j] == 4) {
				poleList.push_back({ pos.x, pos.y });
			}
			else if (gen[i][j] == 5) {
				std::shared_ptr<Trapdoor> newTrapdoor = std::make_shared<Trapdoor>(Trapdoor({ pos.x, pos.y }));

				trapdoorList.push_back(newTrapdoor);
				trapdoors[i][j] = newTrapdoor;
			}
			else if (gen[i][j] == 6) {
				finishingLadders.push_back({ pos.x, pos.y });
			}
			else if (gen[i][j] == 7) {
				std::shared_ptr<Gold> gold = std::make_shared<Gold>(pos);
				uncollectedGoldList.push_back(gold);
			}
			else if (gen[i][j] == 8) {
				std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(pos.x, pos.y);
				enemy->setGameContext(this);
				enemy->setCharSpeed(gameConfiguration->getEnemySpeed());
				enemies.push_back(enemy);
			}
			else if (gen[i][j] == 9) {
				if (player == nullptr) {
					player = std::make_shared<Player>(pos.x, pos.y);
					player->setGameContext(this);
					player->setCharSpeed(gameConfiguration->getPlayerSpeed());
					player->setIOContext(ioContext);
				}
			}

			layout[i][j] = generatorLayoutMap.at(gen[i][j]);
		}
	}

	//if no player was given, put in one!
	if (player == nullptr) {
		for (int j = 1; j < 18; j++) {
			if (player) {
				break;
			}

			for (int i = 1; i < 30; i++) {
				if (layout[i][j] == LayoutBlock::empty && gen[i][j] != 7 && gen[i][j] != 8) {
					Vector2DInt pos = { i, j };

					player = std::make_shared<Player>(pos.x, pos.y);
					player->setGameContext(this);
					player->setCharSpeed(gameConfiguration->getPlayerSpeed());
					player->setIOContext(ioContext);
					break;
				}
			}
		}
	}

	if (player) {
		highestLadder = highestLadder < 15 ? 15 : highestLadder;

		timeText = std::make_shared<Text>(Text("GAMETIME: 0.0 SEC   ", { -5, 0 }));

		std::vector<std::shared_ptr<Text>> textList;
		textList.push_back(timeText);

		enemies.insert(enemies.end(), player);

		renderingManager->clearRenderableObjects();
		renderingManager->setPoleList(poleList);
		renderingManager->setConcreteList(concreteList);
		renderingManager->setBrickList(brickList);
		renderingManager->setTrapdoorList(trapdoorList);
		renderingManager->setLadderList(ladderList);
		renderingManager->setFinishingLadderList(finishingLadders);
		renderingManager->setGoldList(uncollectedGoldList);
		renderingManager->initializeLevelLayout();

		renderingManager->setEnemyList(enemies);
		renderingManager->initializeEnemies();

		renderingManager->setTextList(textList);
		renderingManager->initializeCharacters();

		enemies.erase(enemies.end() - 1);
	}
}

void GameContext::resetSessionLength()
{
	previousSessionSum = 0.0f;
}

void GameContext::setSessionStartTime()
{
	sessionStartTime = std::chrono::system_clock::now();
	previousFrame = sessionStartTime;
}

void GameContext::setSessionEndTime()
{
	std::chrono::duration<float, std::milli> workTime = std::chrono::system_clock::now() - sessionStartTime;
	previousSessionSum += workTime.count() / 1000;
}

float GameContext::calculateEllapsedTime()
{
	std::chrono::duration<float, std::milli> workTime = std::chrono::system_clock::now() - sessionStartTime;
	return previousSessionSum + workTime.count() /1000;
}

float GameContext::calculateFrameDelta()
{
	auto currentFrame = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> frameDelta = currentFrame  - previousFrame;
	previousFrame = currentFrame;
	return frameDelta.count() / 1000;
}