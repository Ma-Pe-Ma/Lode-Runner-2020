#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Audio.h"

class GamePlay;

class GameState {
protected:
	static GamePlay* gamePlay;
	float startTime = 0;

public:
	static void initialize(GamePlay*);
	void setGamePlay(GamePlay*);
	virtual void start() = 0;
	virtual void update(float) = 0;
	virtual void end() = 0;
};
#endif // !GAMESTATE_H