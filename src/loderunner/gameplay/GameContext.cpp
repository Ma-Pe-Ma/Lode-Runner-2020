#include "GameContext.h"

#include "iocontext/rendering/RenderingManager.h"
#include "states/gamestates/Play.h"

#include "Player.h"
#include "Gold.h"

void GameContext::run() {
	float gameTime = calculateEllapsedTime();
	float frameDelta = calculateFrameDelta();

	//set flashing state of ladders
	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;
	getRenderingManager()->setLadderFlashFactor(ladderFactor);

	//Write the current time on screen
	std::string timeValue = std::to_string(gameTime);
	timeValue = timeValue.substr(0, timeValue.length() - 5);
	timeText->changeContent("GAMETIME: " + timeValue + " SEC");

	//handle player and enemy interactions
	player->handle(gameTime, frameDelta);

	for (auto& enemy : enemies) {
		enemy->handle(gameTime, frameDelta);
	}

	//handler brick interactions
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

void GameContext::loadLevel(int levelNumber)
{
	clearContainers();
	
	bricks = std::make_shared<std::shared_ptr<std::shared_ptr<Brick>[]>[]>(30);
	trapdoors = std::make_shared<std::shared_ptr<std::shared_ptr<Trapdoor>[]>[]>(30);;

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;

	for (int i = 0; i < 30; i++) {		
		bricks[i] = std::make_shared<std::shared_ptr<Brick>[]>(18);
		trapdoors[i] = std::make_shared<std::shared_ptr<Trapdoor>[]>(18);

		for (int j = 0; j < 18; j++) {
			if (i == 0 || i == 29 || j == 0) {
				concreteList.push_back({ i, j });
				layout[i][j] = LayoutBlock::concrete;
			}
			else {
				layout[i][j] = LayoutBlock::empty;
			}
		}
	}	

	gameConfiguration->validateLevel(levelNumber);

	std::string levelName = std::to_string(levelNumber);
	levelName = "Level " + levelName.insert(0, 3 - levelName.length(), '0');

	const std::string levelFileName = gameConfiguration->getLevelFileName();

	std::array<std::array<char, 28>, 16> levelLayout = ioContext->loadLevel(levelFileName, levelNumber);

	for (int j = 1; j < levelLayout.size() + 1; j++) {
		std::array<char, 28>& row = levelLayout[16 - j];

		for (int i = 1; i < row.size() + 1; i++) {
			std::shared_ptr<Brick> newBrick;
			std::shared_ptr<Trapdoor> trapdoor;
			std::shared_ptr<Enemy> enemy;
			std::shared_ptr<Gold> gold;
			
			switch (row[i - 1]) {
			case '&':	//runner
				layout[i][j] = LayoutBlock::empty;

				player = std::make_shared<Player>(float(i), float(j));
				player->setGameContext(this);
				player->setCharSpeed(gameConfiguration->getPlayerSpeed());
				player->setIOContext(getIOContext());
				break;
			case '0':	//enemy
				layout[i][j] = LayoutBlock::empty;

				enemy = std::make_shared<Enemy>(float(i), float(j));
				enemy->setGameContext(this);
				enemy->setCharSpeed(gameConfiguration->getEnemySpeed());
				enemies.push_back(enemy);
				break;
			case '#':	//brick
				layout[i][j] = LayoutBlock::brick;
				newBrick = std::make_shared<Brick>(Vector2DInt{ i, j });
				newBrick->setGameContext(this);

				brickList.push_back(newBrick);
				bricks[i][j] = newBrick;
				break;
			case 'X':	//trapdoor
				layout[i][j] = LayoutBlock::trapDoor;

				trapdoor = std::make_shared<Trapdoor>(Trapdoor({ i, j}));
				trapdoorList.push_back(trapdoor);
				trapdoors[i][j] = trapdoor;
				break;
			case '@':	//concrete
				layout[i][j] = LayoutBlock::concrete;
				concreteList.push_back({ i, j });
				break;
			case 'H':	//ladder
				layout[i][j] = LayoutBlock::ladder;
				ladderList.push_back({ i, j });

				if (j > highestLadder) {
					highestLadder = j;
				}

				break;
			case 'S':	//finishing ladder
				layout[i][j] = LayoutBlock::empty;
				finishingLadders.push_back({ i, j });

				if (j > highestLadder) {
					highestLadder = j;
				}

				break;
			case '-':	//vertical pole
				layout[i][j] = LayoutBlock::pole;
				poleList.push_back({ i, j });
				break;
			case '$':	//gold
				layout[i][j] = LayoutBlock::empty;

				gold = std::make_shared<Gold>(Gold({ i, j }));
				uncollectedGoldList.push_back(gold);
				break;
			default:
				layout[i][j] = LayoutBlock::empty;
			}
		}		
	}

	timeText = std::make_shared<Text>(Text("GAMETIME: 0.0 SEC   ", { -5, 0 }));
	std::vector<std::shared_ptr<Text>> textList{
		timeText
	};

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
	this->pointerToDebrisTexture = renderingManager->getPointerToDebrisTexture();
	this->pointerToDebrisLocation = renderingManager->getPointerToDebrisLocation();

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

void GameContext::generateLevel(std::array<std::array<short, 28>, 16> gen)
{
	clearContainers();

	bricks = std::make_shared<std::shared_ptr<std::shared_ptr<Brick>[]>[]>(30);
	trapdoors = std::make_shared<std::shared_ptr<std::shared_ptr<Trapdoor>[]>[]>(30);;


	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;

	for (int i = 0; i < 30; i++) {
		bricks[i] = std::make_shared<std::shared_ptr<Brick>[]>(18);
		trapdoors[i] = std::make_shared<std::shared_ptr<Trapdoor>[]>(18);

		for (int j = 0; j < 18; j++)
		{
			if (j == 0 || i == 0 || i == 29) {
				layout[i][j] = LayoutBlock::concrete;
				concreteList.push_back({ i, j });
			}
			else {
				layout[i][j] = LayoutBlock::empty;
			}			
		}
	}

	for (int i = 0; i < 28; i++) {
		for (int j = 0; j < 16; j++) {
			Vector2DInt pos = { i + 1, j + 1};

			if (gen[j][i] == 1) {
				std::shared_ptr<Brick> newBrick = std::make_shared<Brick>(Brick({ pos.x, pos.y }));

				brickList.push_back(newBrick);
				bricks[i + 1][j + 1] = newBrick;

				newBrick->setGameContext(this);
			}
			else if (gen[j][i] == 2) {
				concreteList.push_back({ pos.x, pos.y });
			}
			else if (gen[j][i] == 3) {
				ladderList.push_back({ pos.x, pos.y });

				if (j > highestLadder) {
					highestLadder = j + 1;
				}
			}
			else if (gen[j][i] == 4) {
				poleList.push_back({ pos.x, pos.y });
			}
			else if (gen[j][i] == 5) {
				std::shared_ptr<Trapdoor> newTrapdoor = std::make_shared<Trapdoor>(Trapdoor({ pos.x, pos.y }));

				trapdoorList.push_back(newTrapdoor);
				trapdoors[i + 1][j + 1] = newTrapdoor;
			}
			else if (gen[j][i] == 6) {
				finishingLadders.push_back({ pos.x, pos.y });

				if (j > highestLadder) {
					highestLadder = j + 1;
				}
			}
			else if (gen[j][i] == 7) {
				std::shared_ptr<Gold> gold = std::make_shared<Gold>(pos);
				uncollectedGoldList.push_back(gold);
			}
			else if (gen[j][i] == 8) {
				std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(pos.x, pos.y);
				enemy->setGameContext(this);
				enemy->setCharSpeed(gameConfiguration->getEnemySpeed());
				enemies.push_back(enemy);
			}
			else if (gen[j][i] == 9) {
				if (player == nullptr) {
					player = std::make_shared<Player>(pos.x, pos.y);
					player->setGameContext(this);
					player->setCharSpeed(gameConfiguration->getPlayerSpeed());
					player->setIOContext(ioContext);
				}
			}

			layout[i + 1][j + 1] = generatorLayoutMap.at(gen[j][i]);
		}
	}

	//if no player was given, put in one!
	if (player == nullptr) {
		for (int j = 1; j < 18; j++) {
			if (player) {
				break;
			}

			for (int i = 1; i < 30; i++) {
				if (layout[i][j] == LayoutBlock::empty && gen[j][i] != 7 && gen[j][i] != 8) {
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
		this->pointerToDebrisTexture = renderingManager->getPointerToDebrisTexture();
		this->pointerToDebrisLocation = renderingManager->getPointerToDebrisLocation();

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