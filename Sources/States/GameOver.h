#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "State.h"
#include <string>

class GameOver : public State {
private:
	std::string playerName;
	const std::string gameOverText = "GAME OVER";
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif