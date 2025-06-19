#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

class Player;
class Enemy;
class Brick;
class Gold;
class Trapdoor;
class Text;

class Play;

class AudioContext;
class RenderingManager;
class IOContext;

#include <map>
#include <memory>
#include <vector>
#include <array>
#include <chrono>

#include "iocontext/GameConfiguration.h"

#include "LayoutBlock.h"
#include "LevelLoader.h"

#include "Vector2DInt.h"

class GameContext {
	std::shared_ptr<RenderingManager> renderingManager;
	std::shared_ptr<GameConfiguration> gameConfiguration;
	std::shared_ptr<AudioContext> audio;
	std::shared_ptr<IOContext> ioContext;

	std::shared_ptr<GameElements> gameElements;

	std::shared_ptr<Text> timeText;

	Play* play;

	std::shared_ptr<LevelLoader> levelLoader;

	float previousSessionSum = 0;
	std::chrono::system_clock::time_point sessionStartTime;
	std::chrono::system_clock::time_point previousFrame;
	float gameTime = 0.0f;
	float frameDelta = 0.0f;

	void handleCharacters();
	void handleDigging();
	void checkGoldCollecting();
	void checkFinishingCondition();

	void renderingTasks();
public:
	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager) { this->renderingManager = renderingManager; }
	std::shared_ptr<RenderingManager> getRenderingManager() { return this->renderingManager; }

	void setGameConfiguration(std::shared_ptr<GameConfiguration> gameConfiguration) { this->gameConfiguration = gameConfiguration; }
	std::shared_ptr<GameConfiguration> getGameConfiguration() { return this->gameConfiguration; }

	void setAudio(std::shared_ptr<AudioContext> audio) { this->audio = audio; }
	std::shared_ptr<AudioContext> getAudio() { return this->audio; }

	void setIOContext(std::shared_ptr<IOContext> ioContext);

	std::shared_ptr<IOContext> getIOContext() { return this->ioContext; }

	void setPlayState(Play* play) { this->play = play; }

	void run();
	void handlePlayerDying(float);

	void generateFinishingLadders();

	void initialize();
	void loadLevel(int);
	void generateLevel(std::array<std::array<short, 28>, 16>);
	
	void resetSessionLength();
	void setSessionStartTime();
	void setSessionEndTime();
	float calculateEllapsedTime();
	float calculateFrameDelta();

	friend class Enemy;
	friend class Player;

	Vector2DInt findPlayerPath();
	Vector2D moveEnemy(std::shared_ptr<Enemy> enemy, float speed, Vector2DInt pathDirection);
	Vector2D enemyCollision(std::vector<std::shared_ptr<Enemy>>::iterator, Vector2D);
	void animate(std::shared_ptr<Enemy> enemy, Vector2D d, float animationTimeFactor, float gameTime, bool player = false);

#ifndef NDEBUG
	Vector2DInt findDebugPath(int);	
#endif NDEBUG
};

#endif