#include "states/Outro.h"
#include "iocontext/audio/AudioFile.h"
#include "states/StateContext.h"

#include "iocontext/rendering/RenderingManager.h"
#include "gameplay/Player.h"
#include <memory>

void Outro::setScoreParameters(short killCounter, short goldCounter, short fruitID) {
	this->enemyScore = killCounter * 200;
	this->goldScore = goldCounter * 200;
	this->fruitID = fruitID;
}

void Outro::setupRenderingManager()
{
	auto renderingManager = stateContext->getRenderingManager();

	renderingManager->clearRenderableObjects();

	//add bricks
	std::vector<std::shared_ptr<Brick>> brickList = {
		std::make_shared<Brick>(Brick({7, 2})),
		std::make_shared<Brick>(Brick({8, 2 })),
		std::make_shared<Brick>(Brick({10, 2 })),
		std::make_shared<Brick>(Brick({11, 2 })),
		std::make_shared<Brick>(Brick({12, 2 }))
	};

	//add ladders
	std::vector<std::tuple<int, int>> ladderList{
		{9 ,0},
		{9 ,1},
		{9 ,2},
	};

	//add gold indicator
	std::vector<std::shared_ptr<Gold>> goldList{
		std::make_shared<Gold>(Gold({11, 13}))
	};

	//add enemy indicator
	std::vector<std::shared_ptr<Enemy>> enemyList{
		std::make_shared<Player>(Player({ 9.0f, 0.0f })),
		std::make_shared<Enemy>(Enemy({11.0f, 9.0f}))
	};

	//climbing runner
	this->player = std::static_pointer_cast<Player>(enemyList[0]);

	std::vector<std::shared_ptr<Text>> textList{
		goldPoints,
		enemyPoints,
		totalPoints
	};

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

	stateContext->getAudio()->getAudioFileByID(13)->playPause();

	int& playerLife = stateContext->playerLife[stateContext->playerNr];
	playerLife = ++playerLife > 9 ? 9 : playerLife;

	int& playerLevel = stateContext->level[stateContext->playerNr];
	playerLevel = ++playerLevel > 150 ? 1 : playerLevel;

	int& playerScore = stateContext->score[stateContext->playerNr];
	playerScore += enemyScore + goldScore;
	stateContext->highScore = playerScore > stateContext->highScore ? playerScore : stateContext->highScore;

	startTimePoint = std::chrono::system_clock::now();
	previousFrame = startTimePoint;
}

void Outro::update() {
	float ellapsedTime = calculateEllapsedTime();

	auto currentFrame = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> frameDeltaDuration = std::chrono::system_clock::now() - previousFrame;
	float frameDelta = frameDeltaDuration.count() / 1000;
	previousFrame = currentFrame;

	int timeFactor = int(2 * ellapsedTime) % 4;
	timeFactor = timeFactor == 3 ? 1 : timeFactor;

	auto renderingManager = stateContext->getRenderingManager();
	renderingManager->setLadderFlashFactor(timeFactor);
	renderingManager->render();

	//runner climbs ladder
	if (this->player->getPos().y < 3.0f) {
		
		if (this->player->getPos().y + frameDelta * 0.5f >= 3.0f) {
			this->player->setPosition({ this->player->getPos().x, 3.0f});
		}
		else {
			this->player->setPosition({ this->player->getPos().x, this->player->getPos().y + frameDelta * 0.5f });
		}
		
		int timeFactor = ellapsedTime * 4;
		int textureRef = 36 + timeFactor % 4;

		this->player->setTexture(textureRef);
	}
	//nail bitting after reaching top of ladder
	else {
		int timeFactor = int(ellapsedTime * 3) % 4;
		this->player->setTexture(44 + timeFactor);
	}

	if (ellapsedTime > stateContext->getAudio()->getAudioFileByID(13)->lengthInSec() || stateContext->getIOContext()->getButtonInputs().enter.simple()) {
		if (stateContext->menuCursor < 2) {
			stateContext->transitionToAtEndOfFrame(stateContext->getIntro());
		}
		else {
			stateContext->transitionToAtEndOfFrame(stateContext->getGenerator());
		}	
	}	
}

void Outro::end() {
	auto audio = stateContext->getAudio();

	for (auto id : std::vector<int>{ 7, 13 })
	{
		audio->getAudioFileByID(id)->stopAndRewind();
	}
}