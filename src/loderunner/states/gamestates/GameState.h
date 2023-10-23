#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "iocontext/audio/AudioContext.h"

#include <memory>
#include <chrono>

class GameContext;
class GameConfiguration;
class IOContext;
class AudioContext;

class GamePlay;

class GameState {
protected:
	GamePlay* gamePlay;

	std::shared_ptr<GameContext> gameContext;

	std::chrono::system_clock::time_point startTimePoint;
	float calculateEllapsedTime();	
public:
	void setGamePlay(GamePlay*);
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void end() = 0;

	virtual void setGameContext(std::shared_ptr<GameContext>);
};
#endif // !GAMESTATE_H