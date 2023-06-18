#include "States/Generator.h"
#include "Audio/AudioFile.h"
#include "Enemy.h"
#include "Gold.h"
#include "Player.h"
#include "States/StateContext.h"

Generator::Generator() {
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

	layoutMap.insert(std::pair<short, LayoutBlock>(0, LayoutBlock::empty));
	layoutMap.insert(std::pair<short, LayoutBlock>(1, LayoutBlock::brick));
	layoutMap.insert(std::pair<short, LayoutBlock>(2, LayoutBlock::concrete));
	layoutMap.insert(std::pair<short, LayoutBlock>(3, LayoutBlock::ladder));
	layoutMap.insert(std::pair<short, LayoutBlock>(4, LayoutBlock::pole));
	layoutMap.insert(std::pair<short, LayoutBlock>(5, LayoutBlock::trapDoor));
	layoutMap.insert(std::pair<short, LayoutBlock>(6, LayoutBlock::empty));
	layoutMap.insert(std::pair<short, LayoutBlock>(7, LayoutBlock::empty));
	layoutMap.insert(std::pair<short, LayoutBlock>(8, LayoutBlock::empty));
	layoutMap.insert(std::pair<short, LayoutBlock>(9, LayoutBlock::empty));

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 18; j++) {
			if (j == 0) {
				gen[i][j] = 2;
				texture[i][j] = textureMap[2];
			}
			else if (i == 0 || i == 29) {
				gen[i][j] = 2;
				texture[i][j] = textureMap[2];
			}
			else {
				gen[i][j] = 0;
				texture[i][j] = textureMap[0];
			}
		}
	}

	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			pos[i][j][0] = i;
			pos[i][j][1] = j;
		}
	}

	geX = 1;
	geY = 16;
}

void Generator::start() {
	stateContext->getRenderingManager()->setGeneratorParameters(&pos[0][0][0], &texture[0][0]);

	getStateContext()->getGamePlay()->getPlay()->clearContainers();
}

void Generator::update(float currentFrame) {
	if (stateContext->getIOContext()->getRightButton().simple()) {
		if (++geX > 28) {
			geX = 28;
		}
		else {
			texture[geX - 1][geY] = textureMap[gen[geX - 1][geY]];
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	if (stateContext->getIOContext()->getLeftButton().simple()) {
		if (--geX < 1) {
			geX = 1;
		}
		else {
			texture[geX + 1][geY] = textureMap[gen[geX + 1][geY]];
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	if (stateContext->getIOContext()->getUpButton().simple()) {
		if (++geY > 16) {
			geY = 16;
		}
		else {
			texture[geX][geY - 1] = textureMap[gen[geX][geY - 1]];
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	if (stateContext->getIOContext()->getDownButton().simple()) {
		if (--geY < 1) {
			geY = 1;
		}
		else {
			texture[geX][geY + 1] = textureMap[gen[geX][geY + 1]];
			stateContext->getAudio()->getAudioFileByID(9)->stopAndRewind();
			stateContext->getAudio()->getAudioFileByID(9)->playPause();
		}
	}

	//chaning element upwards
	if (stateContext->getIOContext()->getRightDigButton().simple()) {
		if (++gen[geX][geY] > 9) {
			gen[geX][geY] = 0;	
		}

		texture[geX][geY] = textureMap[gen[geX][geY]];

		stateContext->getAudio()->getAudioFileByID(10)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(10)->playPause();
	}

	//chaning element stateContext->getIOContext()->downwards
	if (stateContext->getIOContext()->getLeftDigButton().simple()) {
		if (--gen[geX][geY] < 0) {
			gen[geX][geY] = 9;
		}

		texture[geX][geY] = textureMap[gen[geX][geY]];

		stateContext->getAudio()->getAudioFileByID(10)->stopAndRewind();
		stateContext->getAudio()->getAudioFileByID(10)->playPause();
	}

	//yellow cursor block
	if (int(3 * currentFrame) % 2) {
		texture[geX][geY] = 54;
	}
	else {
		texture[geX][geY] = textureMap[gen[geX][geY]];
	}
	
	stateContext->getRenderingManager()->renderGenerator();

	if (stateContext->getIOContext()->getEnterButton().simple()) {
		texture[geX][geY] = textureMap[gen[geX][geY]];

		generateLevel();
	}
}

void Generator::end() {

}

void Generator::generateLevel()
{
	int highestLadder = 0;

	LayoutBlock** layout = new LayoutBlock * [30];
	std::shared_ptr<Brick>** bricks = new std::shared_ptr<Brick>*[30];
	std::shared_ptr<Trapdoor>** trapdoors = new std::shared_ptr<Trapdoor>*[30];

	for (int i = 0; i < 30; i++) {
		layout[i] = new LayoutBlock[18]{};
		bricks[i] = new std::shared_ptr<Brick>[18];
		trapdoors[i] = new std::shared_ptr<Trapdoor>[18];
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

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 18; j++) {
			layout[i][j] = layoutMap[gen[i][j]];

			Vector2DInt pos = { i, j };

			if (gen[i][j] == 1) {
				std::shared_ptr<Brick> newBrick = std::make_shared<Brick>(Brick({ pos.x, pos.y }));

				brickList.push_back(newBrick);
				bricks[i][j] = newBrick;

				newBrick->setGameContext(gameContext);
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
				std::shared_ptr<Trapdoor> newTrapdoor = std::make_shared<Trapdoor>(Trapdoor({pos.x, pos.y}));
				
				trapDoorList.push_back(newTrapdoor);
				trapdoors[i][j] = newTrapdoor;
			}
			else if (gen[i][j] == 6) {
				finishingLadderList.push_back({ pos.x, pos.y });
			}
			else if (gen[i][j] == 7) {
				std::shared_ptr<Gold> gold = std::make_shared<Gold>(pos);				
				goldList.push_back(gold);
			}
			else if (gen[i][j] == 8) {
				std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(pos.x, pos.y);
				enemy->setGameContext(gameContext);
				enemy->setCharSpeed(stateContext->getGameConfiguration()->getEnemySpeed());
				enemyList.push_back(enemy);
			}
			else if (gen[i][j] == 9) {
				if (player == nullptr) {
					player = std::make_shared<Player>(pos.x, pos.y);
					player->setGameContext(gameContext);
					player->setCharSpeed(stateContext->getGameConfiguration()->getPlayerSpeed());
					player->setIOContext(gameContext->getIOContext());
				}
			}

			layout[i][j] = layoutMap[gen[i][j]];
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
					player->setGameContext(gameContext);
					player->setCharSpeed(stateContext->getGameConfiguration()->getPlayerSpeed());
					player->setIOContext(gameContext->getIOContext());
					break;
				}
			}
		}
	}

	if (player) {
		highestLadder = highestLadder < 15 ? 15 : highestLadder;

		timeText = std::make_shared<Text>(Text("GAMETIME: 0.0 SEC   ", { -5, 0 }));
		stateContext->getGamePlay()->getPlay()->setTimeText(timeText);

		std::vector<std::shared_ptr<Text>> textList;
		textList.push_back(timeText);

		enemyList.insert(enemyList.end(), player);

		gameContext->setHighestLadder(highestLadder);

		//stateContext->getRenderingManager()->setGameContext(gameContext);
		//gameContext->setRenderingManager(renderingManager);

		stateContext->getRenderingManager()->clearRenderableObjects();
		stateContext->getRenderingManager()->setPoleList(poleList);
		stateContext->getRenderingManager()->setConcreteList(concreteList);
		stateContext->getRenderingManager()->setBrickList(brickList);
		stateContext->getRenderingManager()->setTrapdoorList(trapDoorList);
		stateContext->getRenderingManager()->setLadderList(ladderList);
		stateContext->getRenderingManager()->setFinishingLadderList(finishingLadderList);
		stateContext->getRenderingManager()->setGoldList(goldList);
		stateContext->getRenderingManager()->initializeLevelLayout();

		stateContext->getRenderingManager()->setEnemyList(enemyList);
		stateContext->getRenderingManager()->initializeEnemies();

		stateContext->getRenderingManager()->setTextList(textList);
		stateContext->getRenderingManager()->initializeCharacters();

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

		//gameContext->setLadders(highestLadder, finisihingLadders);
		stateContext->getGamePlay()->setGameContext(gameContext);
		stateContext->transitionToAtEndOfFrame(stateContext->getGamePlay());
	}
}