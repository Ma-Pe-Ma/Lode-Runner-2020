#include "States/Outro.h"
#include "Audio.h"
#include "GameTime.h"
#include "States/StateContext.h"

#include "Rendering/RenderingManager.h"
#include "../Player.h"
#include <memory>

void Outro::setScoreParameters(short killCounter, short goldCounter, short fruitID) {
	this->enemyScore = killCounter * 200;
	this->goldScore = goldCounter * 200;
	this->fruitID = fruitID;
}

void Outro::setupRenderingManager()
{
	renderingManager->clearRenderableObjects();

	//add bricks
	std::shared_ptr<std::vector<std::shared_ptr<Brick>>> brickList = std::make_shared<std::vector<std::shared_ptr<Brick>>>();
	brickList->push_back(std::make_shared<Brick>(Brick({7, 2})));
	brickList->push_back(std::make_shared<Brick>(Brick({8, 2 })));
	brickList->push_back(std::make_shared<Brick>(Brick({10, 2 })));
	brickList->push_back(std::make_shared<Brick>(Brick({11, 2 })));
	brickList->push_back(std::make_shared<Brick>(Brick({12, 2 })));

	//add ladders
	std::vector<std::tuple<int, int>> ladderList;
	ladderList.push_back({ 9, 0 });
	ladderList.push_back({ 9, 1 });
	ladderList.push_back({ 9, 2 });

	//add gold indicator
	std::vector<std::shared_ptr<Gold>> goldList;
	goldList.push_back(std::make_shared<Gold>(Gold({11, 13})));

	//add enemy indicator
	std::vector<std::shared_ptr<Enemy>> enemyList;
	enemyList.push_back(std::make_shared<Enemy>(Enemy({11, 9})));

	//add climbing runner
	this->player = std::make_shared<Player>(Player({ 9, 0 }));
	enemyList.push_back(this->player);

	std::vector<std::shared_ptr<Text>> textList;
	textList.push_back(goldPoints);
	textList.push_back(enemyPoints);
	textList.push_back(totalPoints);

	renderingManager->setBrickList(brickList);
	renderingManager->setLadderList(ladderList);
	renderingManager->setGoldList(goldList);

	renderingManager->setEnemyList(enemyList);

	renderingManager->setTextList(textList);

	renderingManager->initializeLevelLayout();
	renderingManager->initializeEnemies();

	renderingManager->initializeCharacters();
}

void Outro::start() {
	goldPoints = std::make_shared<Text>(Text(std::to_string(goldScore).append(" POINTS"), { 20, 6.75f }));
	enemyPoints = std::make_shared<Text>(Text(std::to_string(enemyScore).append(" POINTS"), { 20, 12.75f }));
	totalPoints = std::make_shared<Text>(Text(std::string("TOTAL ").append(std::to_string(goldScore + enemyScore)).append(" POINTS"), { 14.5f, 23.25f }));

	setupRenderingManager();

	Audio::sfx[13].playPause();

	int& playerLife = stateContext->playerLife[stateContext->playerNr];
	playerLife = ++playerLife > 9 ? 9 : playerLife;

	int& playerLevel = stateContext->level[stateContext->playerNr];
	playerLevel = ++playerLevel > 150 ? 1 : playerLevel;

	int& playerScore = stateContext->score[stateContext->playerNr];
	playerScore += enemyScore + goldScore;
	stateContext->highScore = playerScore > stateContext->highScore ? playerScore : stateContext->highScore;

	timer = GameTime::getCurrentFrame();
}

void Outro::update(float currentFrame) {
	int timeFactor = int(2 * currentFrame) % 4;
	timeFactor = timeFactor == 3 ? 1 : timeFactor;
	
	renderingManager->setLadderFlashFactor(timeFactor);
	renderingManager->render();

	//runner climbs ladder
	if (this->player->pos.y + GameTime::getSpeed() * 0.1f < 3) {
		
		if (this->player->pos.y + GameTime::getSpeed() * 0.1f > 3) {
			this->player->setPosition({ this->player->pos.x, 3});
		}
		else {
			this->player->setPosition({ this->player->pos.x, this->player->pos.y + GameTime::getSpeed() * 0.1f });
		}
		
		int timeFactor = ((currentFrame - timer) * 4);
		int textureRef = 36 + timeFactor % 4;

		this->player->setTextureRef(textureRef);
	}
	//nail bitting after reaching top of ladder
	else {
		int timeFactor = int((currentFrame - timer) * 3) % 4;
		this->player->setTextureRef(44 + timeFactor);
	}

	if (GameTime::getCurrentFrame() - timer > Audio::sfx[13].lengthInSec() || enter.simple()) {
		if (stateContext->menuCursor < 2) {
			stateContext->transitionTo(stateContext->intro);
		}
		else {
			stateContext->transitionTo(stateContext->generator);
		}	
	}	
}

void Outro::end() {
	Audio::sfx[7].stopAndRewind();
	Audio::sfx[13].stopAndRewind();
}