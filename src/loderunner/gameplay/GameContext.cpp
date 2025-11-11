#include "GameContext.h"

#include "iocontext/rendering/RenderingManager.h"
#include "states/gamestates/Play.h"

#include <set>

#include "Gold.h"
#include "Enemy.h"

void GameContext::setIOContext(std::shared_ptr<IOContext> ioContext) {
	this->ioContext = ioContext;
	this->levelLoader = std::make_shared<LevelLoader>(ioContext, renderingManager, this);
}

void GameContext::run() {
	gameTime = calculateEllapsedTime();
	frameDelta = calculateFrameDelta();

	handleDigging();
	handleCharacters();

	checkGoldCollecting();
	checkFinishingCondition();

	renderingTasks();
}

void GameContext::handleCharacters() {
	auto playerSpeed = gameConfiguration->getPlayerSpeed() * frameDelta * 5.0f;
	auto enemySpeed = gameConfiguration->getEnemySpeed() * frameDelta * 5.0f;

	auto playerDirection = findPlayerPath();
	gameElements->player->dPos = moveEnemy(gameElements->player, playerSpeed, playerDirection);
	gameElements->player->setPosition({ gameElements->player->pos.x + gameElements->player->dPos.x, gameElements->player->pos.y + gameElements->player->dPos.y });
	gameElements->player->determineNearbyObjects(gameElements, gameElements->player->dPos);

	for (auto& enemy : gameElements->enemies) {
		if (std::abs(enemy->pos.x - gameElements->player->pos.x) < 0.5f && std::abs(enemy->pos.y - gameElements->player->pos.y) < 0.5f) {
			play->transitionToDeath();
			return;
		}
	}

	animate(gameElements->player, gameElements->player->dPos, 4 * playerSpeed / frameDelta, gameTime, true);

	//runner idle animation
	if (gameElements->player->dPos.x != 0) {
		gameElements->player->timer = gameTime;
	}
	else if (gameElements->player->dPos.y == 0 && gameTime - gameElements->player->timer > 1.5f &&
		(gameElements->player->middle != LayoutBlock::pole) && gameElements->player->pos.y == gameElements->player->current.y) {
		*gameElements->player->texturePointer = gameElements->player->enemyTextureMap.idle + (int (gameTime)) % 2;
	}

	float enemyFactor = 4 * enemySpeed / frameDelta;
	for (auto enemyIt = gameElements->enemies.begin(); enemyIt != gameElements->enemies.end(); ++enemyIt) {
		auto& enemy = *enemyIt;

#ifndef NDEBUG		
		int index = enemyIt - gameElements->enemies.begin();
		auto enemyPathDirection = index < 2 && *(gameConfiguration->getEnemyDebugState()) ? findDebugPath(index) : enemy->findPath(gameElements);
		enemy->dPos = moveEnemy(enemy, enemySpeed, enemyPathDirection);
#else
		enemy->dPos = moveEnemy(enemy, enemySpeed, enemy->findPath(gameElements));
#endif // !NDEBUG		
	}

	//collision is in different loop, as incrementation changing inside the loop breaks the collision
	for (auto enemyIt = gameElements->enemies.begin(); enemyIt != gameElements->enemies.end(); ++enemyIt) {
		auto& enemy = *enemyIt;
		auto cPos = enemyCollision(enemyIt, enemy->dPos);
		enemy->setPosition({ enemy->pos.x + cPos.x, enemy->pos.y + cPos.y });
		enemy->determineNearbyObjects(gameElements, cPos);
		int enemyAnimationFactor = int(enemyFactor * (gameTime - enemy->timer)) % 4;
		animate(enemy, cPos, enemyFactor, gameTime, false);
	}
}

void GameContext::checkGoldCollecting() {
	if (gameElements->uncollectedGoldList.size() != 0) {
		auto goldIt = gameElements->uncollectedGoldList.begin();

		while (goldIt != gameElements->uncollectedGoldList.end()) {
			if (std::abs(gameElements->player->pos.x - (*goldIt)->pos.x) < 0.15f && std::abs(gameElements->player->pos.y - (*goldIt)->pos.y) < 0.15f) {
				if (audio->getAudioFileByID(0)->getPlayStatus() == AudioStatus::playing) {
					audio->getAudioFileByID(0)->stopAndRewind();
				}

				audio->getAudioFileByID(0)->playPause();
				this->gameElements->collectedGoldList.push_back(*goldIt);

				(*goldIt)->positionPointer[0] *= -1;
				(*goldIt)->pos.x *= -1;

				goldIt = gameElements->uncollectedGoldList.erase(goldIt);

				//if every gold is collected draw the ladders which are needed to finish the level
				if (gameElements->uncollectedGoldList.size() == 0 && gameElements->enemyGoldList.size() == 0) {
					generateFinishingLadders();
					return;
				}

				continue;
			}

			bool collectedGold = false;

			for (auto& enemy : gameElements->enemies) {
				if (!enemy->goldCounter.has_value() && std::abs(enemy->pos.x - (*goldIt)->pos.x) < 0.15f && std::abs(enemy->pos.y - (*goldIt)->pos.y) < 0.15f) {
					gameElements->enemyGoldList.push_back(*goldIt);

					int releaseCount = ioContext->generateRandomNumberBetween(4, 10) + 5;
					enemy->goldCounter.emplace(releaseCount);

					(*goldIt)->positionPointer[0] = -10.0f;
					(*goldIt)->pos.x = -10.0f;

					goldIt = gameElements->uncollectedGoldList.erase(goldIt);

					collectedGold = true;
					break;
				}
			}

			if (!collectedGold) {
				++goldIt;
			}
		}
	}

	if (gameElements->enemyGoldList.size() != 0) {
		//releasing gold
		for (auto& enemy : gameElements->enemies) {
			if (enemy->goldCounter.has_value()) {
				int prevX = int(enemy->pos.x - enemy->dPos.x + 0.5f);
				int prevY = int(enemy->pos.y - enemy->dPos.y + 0.5f);

				int nextX = int(enemy->pos.x + 0.5f);
				int nextY = int(enemy->pos.y + 0.5f);

				if (prevX != nextX || prevY != nextY) {
					if (enemy->goldCounter.value() != 0) {
						--enemy->goldCounter.value();
					}
					else {
						const std::vector<LayoutBlock> solids = { LayoutBlock::brick, LayoutBlock::concrete, LayoutBlock::ladder };
						LayoutBlock prevBlockUnder = gameElements->layout[prevX][prevY - 1];
						bool solidUnder = std::find_if(solids.begin(), solids.end(), [prevBlockUnder](LayoutBlock element) {
							return element == prevBlockUnder; 
							}) != solids.end();

						LayoutBlock prevBlock = gameElements->layout[prevX][prevY];

						if (prevBlock == LayoutBlock::empty && !gameElements->isGold(prevX, prevY) && !gameElements->isBrick(prevX, prevY) && solidUnder) {
							auto& droppedGold = gameElements->enemyGoldList[0];
							droppedGold->setPos({ float(prevX), float(prevY) });
							gameElements->uncollectedGoldList.push_back(droppedGold);
							gameElements->enemyGoldList.erase(gameElements->enemyGoldList.begin());
							enemy->goldCounter.reset();
						}
					}
				}
			}
		}
	}	
}

void GameContext::handleDigging() {
	// reveal trapdoors if player falls through them
	if (gameElements->player->state == EnemyState::falling) {
		auto trapdoor = gameElements->trapdoorList.begin();
		while (trapdoor != gameElements->trapdoorList.end()) {
			auto trapdoorPos = trapdoor->get()->getPos();
			if (gameElements->player->pos.x == trapdoorPos.x && std::abs(gameElements->player->pos.y - trapdoorPos.y) < 0.5f) {
				trapdoor->get()->setRevealed();				
				trapdoor = gameElements->trapdoorList.erase(trapdoor);
			}
			else {
				++trapdoor;
			}
		}
	}	

	auto enemyPath = gameConfiguration->getEnemySpeed() * frameDelta * 5.0f;

	const std::function<void(std::shared_ptr<Enemy>, int, int)> goldDrop = [this](std::shared_ptr<Enemy> enemy, int x, int y) -> void {
		if (enemy->goldCounter.has_value()) {
			if (gameElements->layout[x][y + 1] != LayoutBlock::brick && !gameElements->isBrick(x, y + 1)) {
				auto carriedGold = gameElements->enemyGoldList[0];
				carriedGold->setPos({ float(x), float(y + 1) });

				gameElements->enemyGoldList.erase(gameElements->enemyGoldList.begin());
				gameElements->uncollectedGoldList.push_back(carriedGold);
				enemy->goldCounter.reset();
			}
			else {
				if (gameElements->uncollectedGoldList.size() == 0 && gameElements->enemyGoldList.size() == 0) {
					gameElements->enemyGoldList.erase(gameElements->enemyGoldList.begin());
					enemy->goldCounter.reset();
					generateFinishingLadders();
				}
			}
		}
		};

	//handle brick interactions
	auto brick = gameElements->brickList.begin();
	while (brick != gameElements->brickList.end()) {
		auto brickState = (*brick)->brickState;
		(*brick)->handle(gameTime);
		renderingManager->setBrickTextureState((*brick)->position.x, (*brick)->position.y, (*brick)->textureState);

		auto x = (*brick)->position.x;
		auto y = (*brick)->position.y;		

		for (auto& enemy : gameElements->enemies) {
			if (std::abs(enemy->pos.x - x) < 0.5f && std::abs(enemy->pos.y - y) < 1.0f) {
				if (brickState == BrickState::rebuilt) {
					if (enemy->pos.y <= y + 0.75f) {
						goldDrop(enemy, x, y);

						++gameElements->killCounter;

						std::vector<int> nonEmptyBlocks;
						int respawnY = 15;

						//finding a row with an empty block starting from the 15th row, and going down
						while (respawnY > 0) {
							for (int i = 1; i < 29; i++) {
								if (gameElements->layout[i][respawnY] == LayoutBlock::empty && !gameElements->isEnemy(i, respawnY) && !gameElements->isGold(i, respawnY) && !gameElements->isBrick(i, respawnY)) {
									nonEmptyBlocks.push_back(i);
								}
							}

							if (nonEmptyBlocks.size() != 0) {
								break;
							}

							respawnY--;
						}

						short randomX = ioContext->generateRandomNumberBetween(0, nonEmptyBlocks.size() - 1);
						enemy->pos.x = nonEmptyBlocks[randomX];
						enemy->pos.y = respawnY;
						enemy->state = EnemyState::dying;
						enemy->timer = gameTime;
					}
					else {
						if (enemy->state == EnemyState::falling) {
							enemy->state = EnemyState::freeRun;
							enemy->pos.y = y + 1;
						}
					}					
				}
				else if (brickState == BrickState::building || brickState == BrickState::watiting) {
					if (enemy->state == EnemyState::falling && enemy->pos.y - enemyPath < y && enemy->pos.y > y && enemy->dPos.y < 0) {
						enemy->timer = gameTime;
						goldDrop(enemy, x, y);
						enemy->pos.y = y;
						enemy->state = EnemyState::pitting;
						
						enemy->hole = Vector2D(x, y);
					}
				}
			}		
		}

		if (std::abs(gameElements->player->pos.x - x) < 0.5f && std::abs(gameElements->player->pos.y - y) < 0.5f) {
			if (brickState == BrickState::rebuilt) {
				gameTime = 0.0f;				
				play->transitionToDeath();
				return;
			}
		}

		if (brickState == BrickState::rebuilt) {
			gameElements->layout[x][y] = LayoutBlock::brick;
			brick = gameElements->brickList.erase(brick);
		}
		else {
			++brick;
		}
	}

	if (!gameElements->diggedBrick) {
		if (gameElements->player->state == EnemyState::freeRun) {
			auto& buttonInputs = ioContext->getButtonInputs();

			std::function<void(int, int, Direction)> startDig = [this](int x, int y, Direction direction) -> void {
				LayoutBlock brickBlock = gameElements->layout[x][y];
				LayoutBlock upBlock = gameElements->layout[x][y + 1];

				if (brickBlock == LayoutBlock::brick && upBlock == LayoutBlock::empty && !gameElements->isGold(x, y + 1)) {

					Vector2DInt pos = { x, y };
					gameElements->diggedBrick = std::make_shared<Brick>(pos, gameTime);
					gameElements->brickList.push_back(gameElements->diggedBrick);

					audio->getAudioFileByID(1)->stopAndRewind();
					audio->getAudioFileByID(1)->playPause();
					gameElements->randomDebris = ioContext->generateRandomNumberBetween(0, 2);

					gameElements->player->timer = gameTime;
					gameElements->player->state = EnemyState::digging;
					gameElements->player->direction = direction;

					if (gameElements->player->middle == LayoutBlock::ladder) {
						*gameElements->player->texturePointer = gameElements->player->enemyTextureMap.ladder;
					}
					else if (gameElements->player->middle == LayoutBlock::pole) {
						*gameElements->player->texturePointer = gameElements->player->enemyTextureMap.pole;
					}
					else {
						*gameElements->player->texturePointer = 24 + (ioContext->generateRandomNumberBetween(0, 4) < 2 ? 0 : 1);
					}
				}
				};

			//Check digging input
			if (buttonInputs.leftDig.impulse()) {
				startDig(gameElements->player->current.x - 1, gameElements->player->current.y - 1, Direction::left);
				return;
			}

			if (buttonInputs.rightDig.impulse()) {
				startDig(gameElements->player->current.x + 1, gameElements->player->current.y - 1, Direction::right);
				return;
			}
		}
	}
	else {
		// brick destroyed
		if (gameElements->diggedBrick->brickState == BrickState::watiting) {
			gameElements->player->state = EnemyState::freeRun;
			gameElements->player->timer = gameTime;
			gameElements->layout[gameElements->diggedBrick->position.x][gameElements->diggedBrick->position.y] = LayoutBlock::empty;
			gameElements->diggedBrick = nullptr;

			renderingManager->setDebrisState(15);
			renderingManager->setDebrisLocation(-1, -1);
		}
		// brick being destroyed
		else {
			// adjust runner to middle
			auto& player = gameElements->player;
			auto playerSpeed = gameConfiguration->getPlayerSpeed() * frameDelta * 5.0f;			

			if (player->pos.x > player->current.x) {
				player->pos.x = player->pos.x - playerSpeed < player->current.x ? player->current.x : player->pos.x - playerSpeed;				
			}
			else if (player->pos.x < player->current.x) {
				player->pos.x = player->pos.x + playerSpeed < player->current.x ? player->current.x : player->pos.x + playerSpeed;
			}

			if (player->pos.y > player->current.y) {
				player->pos.y = player->pos.y - playerSpeed < player->current.y ? player->current.y : player->pos.y - playerSpeed;
			}
			else if (player->pos.y < player->current.y) {
				player->pos.y = player->pos.y + playerSpeed < player->current.y ? player->current.y : player->pos.y + playerSpeed;
			}

			auto x = gameElements->diggedBrick->position.x;
			auto y = gameElements->diggedBrick->position.y;
			//drawing debris above hole
			if (gameElements->layout[x][y + 1] != LayoutBlock::brick) {
				int timeFactor = int(5 * (gameTime - gameElements->player->timer) / 0.5f) % 5;
				renderingManager->setDebrisState(19 + gameElements->randomDebris * 6 + timeFactor);
				renderingManager->setDebrisLocation(x, y + 1);
			}

			// checking if enemy should prevent digging
			for (auto& enemy : gameElements->enemies) {
				if (std::abs(enemy->pos.x - x) <= 0.75f && y + 1 <= enemy->pos.y && enemy->pos.y < y + 1.5f) {
					audio->getAudioFileByID(2)->playPause();
					audio->getAudioFileByID(1)->stopAndRewind();
					gameElements->layout[x][y] = LayoutBlock::brick;
					
					gameElements->player->state = EnemyState::freeRun;
					gameElements->player->timer = gameTime;

					renderingManager->setBrickTextureState(x, y, 0);
					
					std::erase(gameElements->brickList, gameElements->diggedBrick);
					gameElements->diggedBrick = nullptr;					

					renderingManager->setDebrisState(0);
					renderingManager->setDebrisLocation(-1, -1);
					break;
				}
			}		
		}
	}	
}

void GameContext::renderingTasks() {
	auto translation = gameConfiguration->getTranslation();
	auto gameTimeTranslation = translation->getTranslationText("gameTime");
	std::get<0>(gameTimeTranslation) = std::vformat(std::get<0>(gameTimeTranslation), std::make_format_args(gameTime));
	timeText->changeContent(std::get<0>(gameTimeTranslation));

	//set flashing state of ladders
	int ladderFactor = int(gameTime) % 4;
	ladderFactor = ladderFactor == 3 ? 1 : ladderFactor;
	getRenderingManager()->setLadderFlashFactor(ladderFactor);
}

void GameContext::checkFinishingCondition() {
	if (gameElements->player->state == EnemyState::freeRun) {
		
		//if every gold collected!
		if (gameElements->uncollectedGoldList.size() == 0 && gameElements->enemyGoldList.size() == 0) {
			float* fruitPosition = gameElements->fruit->position;

			if (gameElements->fruit->id.has_value() && !gameElements->fruit->collected.has_value()) {
				float ellapsedFruitTime = gameTime - std::get<0>(gameElements->fruit->time);
				bool inTime = ellapsedFruitTime <= std::get<1>(gameElements->fruit->time);

				if (inTime) {
					if (std::abs(gameElements->player->pos.x - fruitPosition[0]) < 0.15f && std::abs(gameElements->player->pos.y - fruitPosition[1]) < 0.15f) {
						if (audio->getAudioFileByID(0)->getPlayStatus() == AudioStatus::playing) {
							audio->getAudioFileByID(0)->stopAndRewind();
						}

						audio->getAudioFileByID(0)->playPause();
						gameElements->fruit->collected.emplace(true);

						gameElements->fruit->position[0] = -1.0f;
						gameElements->fruit->position[1] = -1.0f;
					}
				}
				else {
					gameElements->fruit->collected.emplace(false);
					gameElements->fruit->position[0] = -1.0f;
					gameElements->fruit->position[1] = -1.0f;
				}

			}			

			//top of level reached
			if (gameElements->player->pos.y >= gameElements->highestLadder + 0.5f) {
				std::optional<Fruit> fruit = gameElements->fruit->id.has_value() ? std::make_optional<Fruit>(*gameElements->fruit) : std::nullopt;
				play->transitionToOutro(gameElements->killCounter, gameElements->collectedGoldList.size(), fruit);
			}
		}
	}
}

void GameContext::handlePlayerDying(float ellapsedTime) {
	float deathLength = audio->getAudioFileByID(3)->lengthInSec();
	int timeFactor = ((int)(9 * (ellapsedTime) / deathLength)) % 9;
	timeFactor = (timeFactor == 8) ? 31 : timeFactor;
	*gameElements->player->texturePointer = gameElements->player->enemyTextureMap.death + timeFactor;
}

void GameContext::generateFinishingLadders() {
	audio->getAudioFileByID(4)->playPause();

	renderingManager->enableFinishingLadderDrawing();

	for (auto finishLadder : gameElements->finishingLadders) {
		int x = std::get<0>(finishLadder);
		int y = std::get<1>(finishLadder);

		gameElements->layout[x][y] = LayoutBlock::ladder;
	}

	gameElements->finishingLadders.clear();

	if (gameElements->fruit->id.has_value()) {
		gameElements->fruit->position[0] *= -1;
		gameElements->fruit->position[1] *= -1;

		std::get<0>(gameElements->fruit->time) = gameTime;
		std::get<1>(gameElements->fruit->time) = 15 + ioContext->generateRandomNumberBetween(0, 10);
	}
}

void GameContext::initialize() {
	renderingManager->clearRenderableObjects();

	auto gameTimeTranslation = getGameConfiguration()->getTranslation()->getTranslationText("gameTime");
	float zeroTime = 0.0f;
	std::get<0>(gameTimeTranslation) = std::vformat(std::get<0>(gameTimeTranslation), std::make_format_args(zeroTime));
	timeText = std::make_shared<Text>(gameTimeTranslation);
	renderingManager->setTextList({timeText});
	renderingManager->initializeCharacters();
}

void GameContext::loadLevel(int levelNumber) {
	initialize();

	gameConfiguration->validateLevel(levelNumber);
	const std::string levelFileName = gameConfiguration->getLevelFileName();

	std::array<std::array<char, 28>, 16> levelLayout = ioContext->loadLevel(levelFileName, levelNumber);
	levelLoader->loadLevel(levelLayout);
	this->gameElements = levelLoader->getGameElements();
}

void GameContext::generateLevel(std::array<std::array<short, 28>, 16> gen) {
	initialize();
	levelLoader->generateLevel(gen);
	this->gameElements = levelLoader->getGameElements();
}

void GameContext::resetSessionLength() {
	previousSessionSum = 0.0f;
}

void GameContext::setSessionStartTime() {
	sessionStartTime = std::chrono::system_clock::now();
	previousFrame = sessionStartTime;
}

void GameContext::setSessionEndTime() {
	std::chrono::duration<float, std::milli> workTime = std::chrono::system_clock::now() - sessionStartTime;
	previousSessionSum += workTime.count() / 1000;
}

float GameContext::calculateEllapsedTime() {
	std::chrono::duration<float, std::milli> workTime = std::chrono::system_clock::now() - sessionStartTime;
	return previousSessionSum + workTime.count() /1000;
}

float GameContext::calculateFrameDelta() {
	auto currentFrame = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> frameDelta = currentFrame  - previousFrame;
	previousFrame = currentFrame;
	return frameDelta.count() / 1000;
}

Vector2DInt GameContext::findPlayerPath() {
	auto& buttonInputs = ioContext->getButtonInputs();
	Vector2DInt playerDirection = { 0, 0 };

	if (buttonInputs.right.continuous() && buttonInputs.left.continuous()) {
		//playerDirection.x = 0;
	}
	else if (buttonInputs.right.continuous()) {
		playerDirection.x = 1;
	}
	else if (buttonInputs.left.continuous()) {
		playerDirection.x = -1;
	}

	if (buttonInputs.up.continuous() && buttonInputs.down.continuous()) {
		//playerDirection.y = 0;
	}
	else if (buttonInputs.up.continuous()) {
		playerDirection.y = 1;
	}
	else if (buttonInputs.down.continuous()) {
		playerDirection.y = -1;
	}

	return playerDirection;
}

Vector2D GameContext::moveEnemy(std::shared_ptr<Enemy> enemy, float speed, Vector2DInt pathDirection) {
	auto pos = enemy->pos;
	Vector2D path = { 0.0f, 0.0f };

	if (enemy->state == EnemyState::freeRun) {
		path = { pathDirection.x * speed, pathDirection.y * speed };
		path = enemy->ladderTransformation(path, gameElements);
		path = enemy->checkCollisionsWithEnvironment(path, gameElements);
		path = enemy->handleDropping(path, gameElements);
	}
	else if (enemy->state == EnemyState::falling) {
		path = enemy->falling(speed, gameElements);
	}
	else if (enemy->state == EnemyState::pitting) {
		//Vector2DInt pathDirection = enemy->findPath(gameElements);
		path = { pathDirection.x * speed, pathDirection.y * speed };
		path = enemy->pitting(path, speed, gameTime, gameElements);
	}
	else if (enemy->state == EnemyState::dying) {
		path = { 0, 0 };

		if (gameTime - enemy->timer > 1) {
			enemy->state = EnemyState::falling;
		}
	}

	return path;
}

Vector2D GameContext::enemyCollision(std::vector<std::shared_ptr<Enemy>>::iterator enemyIt, Vector2D d) {
	std::shared_ptr<Enemy> enemy = *enemyIt;
	bool checkPit = int(enemy->pos.x) != int(enemy->pos.x + d.x);

	for (auto otherEnemyIt = gameElements->enemies.begin(); otherEnemyIt != gameElements->enemies.end(); ++otherEnemyIt) {
		if (otherEnemyIt == enemyIt) {
			continue;
		}

		std::shared_ptr<Enemy>& otherEnemy = *otherEnemyIt;

		//stop if other enemy under is moving!
		if (otherEnemy->state != EnemyState::pitting && std::abs(enemy->pos.x - otherEnemy->pos.x) < 1.0f && otherEnemy->pos.y == enemy->pos.y - 1 && otherEnemy->dPos.x != 0 &&
			enemy->middle != LayoutBlock::ladder && enemy->middle != LayoutBlock::pole && enemy->downBlock != LayoutBlock::ladder) {
			return { 0.0f, 0.0f };
		}

		//check X collision
		if (d.x != 0 && std::abs(otherEnemy->pos.x - (enemy->pos.x + d.x)) < 1.0f && std::abs(enemy->pos.y - otherEnemy->pos.y) < 1.0f) {
			//stop if enemy is coming out from hole
			if (checkPit && enemy->state == EnemyState::pitting) {
			 	d.x = enemy->current.x - enemy->pos.x;
				return d;
			}
			
			float scalar = otherEnemy->dPos.x * d.x;

			//opposite direction
			if (scalar < 0.0f) {
				//enemy is polite with the other enemy if the other in the list is earlier
				//this prevents collision when ladder/hole is approached from two sides at the same time
				if (otherEnemyIt < enemyIt) {
					d.x = 0.0f;
				}
			}
			//same direction or other enemy is stopped
			else  { 
				if (std::abs(enemy->pos.x - otherEnemy->pos.x) >= 1.0f) {
					float dxMagn = std::abs(d.x);
					float differenceX = otherEnemy->pos.x - enemy->pos.x;
					d.x = differenceX + (differenceX > 0.0f ? -1.0f : 1.0f);

					if (std::abs(d.x) > dxMagn) {
						d.x = 0.0f;
					}
				}
			}
		}

		//check Y collision
		if (d.y != 0 && std::abs(otherEnemy->pos.y - (enemy->pos.y + d.y)) < 1.0f && std::abs(enemy->pos.x - otherEnemy->pos.x) < 1.0f) {
			float scalar = otherEnemy->dPos.y * d.y;
			if (scalar < 0.0f) {
				if (otherEnemyIt < enemyIt) {
					d.y = 0.0f;
				}
			}
			else if (scalar == 0.0f) {
				if (std::abs(enemy->pos.y - otherEnemy->pos.y) >= 1.0f) {
					float dyMagn = std::abs(d.y);
					float differenceY = otherEnemy->pos.y - enemy->pos.y;
					d.y = differenceY + (differenceY > 0.0f ? -1.0f : 1.0f);

					if (std::abs(d.y) > dyMagn) {
						d.y = 0.0f;
					}
				}
			}
			else {
				if (d.y > 0.0f && otherEnemy->pos.y > enemy->pos.y) {
					d.y = 0.0f;
				}
				else if (d.y < 0.0f && otherEnemy->pos.y < enemy->pos.y) {
					d.y = 0.0f;
				}
			}
		}
	}

	return d;
}

void GameContext::animate(std::shared_ptr<Enemy> enemy, Vector2D d, float animationTimeFactor, float gameTime, bool player)  {
	std::function<void(int)> playSound = [this](int id) -> void {
		if (audio->getAudioFileByID(id + gameElements->going)->getPlayStatus() == AudioStatus::stopped) {
			audio->getAudioFileByID(id + 1 - gameElements->going)->playPause();
			gameElements->going = 1 - gameElements->going;
		}
		};
	
	animationTimeFactor = int(animationTimeFactor * gameTime) % 4;

	switch (enemy->state) {
	case EnemyState::freeRun:
		//going animation
		if (d.x != 0 && (enemy->middle == LayoutBlock::empty || enemy->middle == LayoutBlock::ladder || enemy->middle == LayoutBlock::trapDoor)) {
			*enemy->texturePointer = enemy->enemyTextureMap.going + animationTimeFactor;

			if (player) {
				playSound(9);
			}
		}
		//on ladder, or coming out from pit
		if (d.y != 0 && (enemy->middle == LayoutBlock::ladder || enemy->downBlock == LayoutBlock::ladder)) {
			*enemy->texturePointer = enemy->enemyTextureMap.ladder + animationTimeFactor;

			if (player) {
				playSound(11);
			}
		}
		//on pole
		if (enemy->middle == LayoutBlock::pole && (d.x != 0 || d.y != 0) && enemy->current.y == enemy->pos.y) {
			*enemy->texturePointer = enemy->enemyTextureMap.pole + animationTimeFactor;

			if (player) {
				playSound(15);
			}
		}

		if (player) {
			auto fallAudio = audio->getAudioFileByID(17);
			if (fallAudio->getPlayStatus() == AudioStatus::playing) {
				fallAudio->stopAndRewind();
			}
		}
		break;
	case EnemyState::falling:
		*enemy->texturePointer = enemy->enemyTextureMap.falling + animationTimeFactor;

		if (player) {
			auto fallAudio = audio->getAudioFileByID(17);
			if (fallAudio->getPlayStatus() == AudioStatus::stopped) {
				fallAudio->playPause();
			}
		}
		break;
	case EnemyState::pitting:
		if (d.y != 0) {
			*enemy->texturePointer = enemy->enemyTextureMap.ladder + animationTimeFactor;
		}
		else  if (d.x != 0) {
			*enemy->texturePointer = enemy->enemyTextureMap.falling;
		}
		break;
	case EnemyState::dying:

		*enemy->texturePointer = enemy->enemyTextureMap.death + int(4 * (gameTime - enemy->timer)) % 4;
		break;
	}
}

#ifndef NDEBUG

Vector2DInt GameContext::findDebugPath(int id) {
	auto& buttonInputs = ioContext->getButtonInputs();

	Vector2DInt result = { 0, 0 };

	if (id == 0) {
		if (buttonInputs.d1Right.continuous()) {
			result.x = 1;
		}
		else if (buttonInputs.d1Left.continuous()) {
			result.x = -1;
		}

		if (buttonInputs.d1Up.continuous()) {
			result.y = 1;
		}
		else if (buttonInputs.d1Down.continuous()) {
			result.y = -1;
		}
	}
	else if (id == 1) {
		if (buttonInputs.d2Right.continuous()) {
			result.x = 1;
		}
		else if (buttonInputs.d2Left.continuous()) {
			result.x = -1;
		}

		if (buttonInputs.d2Up.continuous()) {
			result.y = 1;
		}
		else if (buttonInputs.d2Down.continuous()) {
			result.y = -1;
		}
	}

	return result;
}

#endif