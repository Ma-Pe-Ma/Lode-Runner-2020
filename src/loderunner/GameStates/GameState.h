#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Audio/Audio.h"

#include <memory>

class GameContext;
class GameConfiguration;
class IOContext;
class Audio;

class GamePlay;

class GameState {
protected:
	GamePlay* gamePlay;
	float startTime = 0;

	std::shared_ptr<GameContext> gameContext;

public:
	void setGamePlay(GamePlay*);
	virtual void start() = 0;
	virtual void update(float) = 0;
	virtual void end() = 0;

	virtual void setGameContext(std::shared_ptr<GameContext>);
};
#endif // !GAMESTATE_H