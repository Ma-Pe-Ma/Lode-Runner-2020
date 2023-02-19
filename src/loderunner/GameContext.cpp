#include "GameContext.h"
#include "GameTime.h"

#include "IOHandler.h"

#include "Rendering/RenderingManager.h"

#include "Player.h"
#include "Gold.h"

#include "GameStates/Play.h"

void GameContext::run() {
	float gameTime = GameTime::getGameTime();

/*#ifndef RELEASE_VERSION
	//(*enemies)[0]->dPos.x = debugPos[0].x;
	//(*enemies)[0]->dPos.y = debugPos[0].y;
	//(*enemies)[1]->dPos.x = debugPos[1].x;
	//(*enemies)[1]->dPos.y = debugPos[1].y;
#endif // !RELEASE_VERSION*/

	player->handle();

	for (auto& enemy : enemies) {
		enemy->handle();
	}

#ifndef RELEASE_VERSION
	//debugPos[0].x = 0;
	//debugPos[0].y = 0;
	//debugPos[1].x = 0;
	//debugPos[1].y = 0;
#endif

	for (auto brick : brickList)
	{
		brick->handle(gameTime);
	}
}

void GameContext::setBrickList(std::vector<std::shared_ptr<Brick>> brickList)
{
	this->brickList = brickList;
}

void GameContext::setTrapdoorList(std::vector<std::shared_ptr<Trapdoor>> trapdoorList)
{
	this->trapdoorList = trapdoorList;
}

void GameContext::setRenderingManager(std::shared_ptr<RenderingManager> renderingManager)
{
	this->renderingManager = renderingManager;
}

void GameContext::setPlayer(std::shared_ptr<Player> player)
{
	this->player = player;
}

std::shared_ptr<Player> GameContext::getPlayer()
{
	return this->player;
}

void GameContext::setEnemies(std::vector<std::shared_ptr<Enemy>> enemies)
{
	this->enemies = enemies;
}

std::vector<std::shared_ptr<Enemy>> GameContext::getEnemies()
{
	return this->enemies;
}

void GameContext::checkDeaths(int x, int y)
{
	player->checkDeath(x, y);

	for (auto& enemy : enemies) {
		enemy->checkDeath(x, y);
	}
}

void GameContext::notifyPlayerAboutDigEnd() {
	player->releaseFromDigging();
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

void GameContext::generateFinishingLadders() {
	renderingManager->enableFinishingLadderDrawing();

	for (auto finishLadder : finishingLadders) {

		int x = std::get<0>(finishLadder);
		int y = std::get<1>(finishLadder);

		layout[x][y] = LayoutBlock::ladder;
	}

	finishingLadders.clear();
}

bool GameContext::checkDigPrevention(int x, int y) {
	for (auto& enemy : enemies) {
		if (std::abs(enemy->getPos().x - x) <= 0.75f && y + 1 <= enemy->getPos().y && enemy->getPos().y < y + 1.5f) {
			return true;
		}
	}

	return false;
}

void GameContext::clearContainers()
{
	highestLadder = 0;
	finishingLadders.clear();

	killCounter = 0;
	enemies.clear();
	uncollectedGoldList.clear();
	collectedGoldList.clear();

	for (auto & brick : brickList)
	{
		brick = nullptr;
	}

	for (auto& trapdoor : trapdoorList)
	{
		trapdoor = nullptr;
	}

	brickList.clear();
	trapdoorList.clear();

	if (bricks != nullptr && trapdoors != nullptr)
	{
		for (int i = 0; i < 30; i++)
		{
			delete[] bricks[i];
			delete[] trapdoors[i];
		}

		delete[] bricks;
		delete[] trapdoors;

		bricks = nullptr;
		trapdoors = nullptr;
	}	
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

void GameContext::addGoldToCollectedList(std::shared_ptr<Gold> collectedGold) {
	collectedGold->setPos(Vector2DInt{ -1, -1 });
	this->collectedGoldList.push_back(collectedGold);
}

void GameContext::addGoldToUncollectedList(std::shared_ptr<Gold> newGold) {
	this->uncollectedGoldList.push_back(newGold);
}

bool GameContext::goldChecker(int x, int y) {
	for (auto& gold : uncollectedGoldList) {
		if (gold->getPos().x == x && gold->getPos().y == y) {
			return true;
		}
	}

	return false;
}

void GameContext::transitionToDeath()
{
	play->transitionToDeath();
}

void GameContext::transitionToOutro()
{
	play->transitionToOutro(killCounter, getCollectedGoldSize(), 0);
}