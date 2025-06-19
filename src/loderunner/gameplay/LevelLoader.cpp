#include "LevelLoader.h"

#include <set>

#include "Brick.h"
#include "Trapdoor.h"

#include "Gold.h"
#include "Enemy.h"

#include "../iocontext/IOContext.h"
#include "GameContext.h"
#include "../iocontext/rendering/RenderingManager.h"

#include "GameElements.h"

LevelLoader::LevelLoader(std::shared_ptr<IOContext> ioContext, std::shared_ptr<RenderingManager> renderingManager, GameContext* gameContext) {
	this->ioContext = ioContext;
	this->gameContext = gameContext;
	this->renderingManager = renderingManager;
}

void LevelLoader::loadLevel(std::array<std::array<char, 28>, 16> levelLayout, bool generating) {
	gameElements = std::make_shared<GameElements>();
	gameElements->highestLadder = generating ? 15 : 0;
	gameElements->killCounter = 0;

	gameElements->player = nullptr;
	gameElements->enemies.clear();
	gameElements->finishingLadders.clear();
	gameElements->uncollectedGoldList.clear();
	gameElements->collectedGoldList.clear();

	gameElements->diggedBrick = nullptr;
	gameElements->brickList.clear();
	gameElements->trapdoorList.clear();
	gameElements->layout = {LayoutBlock::empty};

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;

	std::array<std::array<bool, 18>, 30> brickMap = { false };

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 18; j++) {
			if (i == 0 || i == 29 || j == 0) {
				concreteList.push_back({ i, j });
				gameElements->layout[i][j] = LayoutBlock::concrete;
			}
		}
	}

	for (int j = 1; j < levelLayout.size() + 1; j++) {
		std::array<char, 28>& row = levelLayout[j - 1];

		for (int i = 1; i < row.size() + 1; i++) {
			std::shared_ptr<Brick> newBrick;
			std::shared_ptr<Trapdoor> trapdoor;
			std::shared_ptr<Enemy> enemy;
			std::shared_ptr<Gold> gold;

			switch (row[i - 1]) {
			case '&':	//runner
				gameElements->player = std::make_shared<Enemy>(float(i), float(j), true);
				break;
			case '0':	//enemy
				gameElements->layout[i][j] = LayoutBlock::empty;
				gameElements->enemies.push_back(std::make_shared<Enemy>(float(i), float(j)));
				break;
			case '#':	//brick
				gameElements->layout[i][j] = LayoutBlock::brick;
				brickMap[i][j] = true;
				break;
			case 'X':	//trapdoor
				gameElements->layout[i][j] = LayoutBlock::trapDoor;

				trapdoor = std::make_shared<Trapdoor>(Trapdoor({ i, j }));
				gameElements->trapdoorList.push_back(trapdoor);
				break;
			case '@':	//concrete
				gameElements->layout[i][j] = LayoutBlock::concrete;
				concreteList.push_back({ i, j });
				break;
			case 'H':	//ladder
				gameElements->layout[i][j] = LayoutBlock::ladder;
				ladderList.push_back({ i, j });

				if (j > gameElements->highestLadder) {
					gameElements->highestLadder = j;
				}

				break;
			case 'S':	//finishing ladder
				gameElements->layout[i][j] = LayoutBlock::empty;
				gameElements->finishingLadders.push_back({ i, j });

				if (j > gameElements->highestLadder) {
					gameElements->highestLadder = j;
				}

				break;
			case '-':	//vertical pole
				gameElements->layout[i][j] = LayoutBlock::pole;
				poleList.push_back({ i, j });
				break;
			case '$':	//gold
				gameElements->layout[i][j] = LayoutBlock::empty;

				gold = std::make_shared<Gold>(Gold({ i, j }));
				gameElements->uncollectedGoldList.push_back(gold);
				break;
			default:
				gameElements->layout[i][j] = LayoutBlock::empty;
			}
		}
	}

	if (gameElements->player == nullptr) {
		for (int j = 1; j < 17; j++) {
			if (gameElements->player) {
				break;
			}

			for (int i = 1; i < 29; i++) {
				if (gameElements->layout[i][j] == LayoutBlock::empty && levelLayout[j - 1][i - 1] != '0' && levelLayout[j - 1][i - 1] != '$') {
					Vector2DInt pos = { i, j };

					gameElements->player = std::make_shared<Enemy>(pos.x, pos.y, true);
					break;
				}
			}
		}
	}

	gameElements->enemies.insert(gameElements->enemies.end(), gameElements->player);

	renderingManager->setPoleList(poleList);
	renderingManager->setConcreteList(concreteList);
	renderingManager->setBrickMap(brickMap);
	renderingManager->setTrapdoorList(gameElements->trapdoorList);
	renderingManager->setLadderList(ladderList);
	renderingManager->setFinishingLadderList(gameElements->finishingLadders);
	renderingManager->setGoldList(gameElements->uncollectedGoldList);
	renderingManager->initializeLevelLayout();
	this->gameElements->pointerToDebrisTexture = renderingManager->getPointerToDebrisTexture();
	this->gameElements->pointerToDebrisLocation = renderingManager->getPointerToDebrisLocation();

	renderingManager->setEnemyList(gameElements->enemies);
	renderingManager->initializeEnemies();

	gameElements->enemies.erase(gameElements->enemies.end() - 1);

	// 50% to generate fruit
	gameElements->fruit = std::make_shared<Fruit>();
	
	if (!generating && ioContext->generateRandomNumberBetween(0, 7) > 3) {
		gameElements->fruit->id = ioContext->generateRandomNumberBetween(0, 7);
		gameElements->fruit->position = renderingManager->setFruitTextureID(60 + gameElements->fruit->id.value());

		std::vector<int> rows(16);
		std::iota(rows.begin(), rows.end(), 1);

		while (rows.size() > 0) {
			int rowID = ioContext->generateRandomNumberBetween(0, rows.size() - 1);
			int row = rows[rowID];

			rows.erase(rows.begin() + rowID);

			std::vector<int> cols(28);
			std::iota(cols.begin(), cols.end(), 1);

			bool breakOuter = false;

			while (cols.size() > 0) {
				int colID = ioContext->generateRandomNumberBetween(0, cols.size() - 1);
				int col = cols[colID];

				cols.erase(cols.begin() + colID);

				std::set<LayoutBlock> solids = std::set<LayoutBlock>({ LayoutBlock::concrete, LayoutBlock::brick, LayoutBlock::ladder, LayoutBlock::finishLadder });

				if (gameElements->layout[col][row] == LayoutBlock::empty && solids.contains(gameElements->layout[col][row - 1])) {
					gameElements->fruit->position[0] = -col;
					gameElements->fruit->position[1] = -row;

					breakOuter = true;
					break;
				}
			}

			if (breakOuter) {
				break;
			}
		}
	}
	else {
		gameElements->fruit->position = renderingManager->setFruitTextureID(60);
		gameElements->fruit->position[0] = -2.0f;
		gameElements->fruit->position[1] = -2.0f;
	}	

	gameElements->player->determineNearbyObjects(gameElements);
	for (auto& enemy : gameElements->enemies) {
		enemy->determineNearbyObjects(gameElements);
	}
}

void LevelLoader::generateLevel(std::array<std::array<short, 28>, 16> gen) {
	std::map<short, char> transformMap = {
		{0, ' '},
		{1, '#'},
		{2, '@'},
		{3, 'H'},
		{4, '-'},
		{5, 'X'},
		{6, 'S'},
		{7, '$'},
		{8, '0'},
		{9, '&'},
	};
	
	std::array<std::array<char, 28>, 16> transformed;

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 28; j++) {
			transformed[i][j] = transformMap[gen[i][j]];
		}
	}

	this->loadLevel(transformed, true);
}