#ifndef OUTRO_H
#define OUTRO_H

#include "State.h"
#include <string>

class Outro : public State {
private:
	std::string gold_points;
	std::string enemy_points;
	std::string total;

	short enemyScore = 0;
	short goldScore = 0;
	short fruitID = 0;
	float runnerY = 0;

public:
	void start() override;
	void update(float) override;
	void end() override;

	void SetScoreParameters(short, short, short);
};

#endif