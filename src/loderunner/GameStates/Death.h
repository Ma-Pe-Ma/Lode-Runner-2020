#ifndef DEATH_H
#define DEATH_H

#include "GameState.h"

class Death : public GameState {
	float timer;

public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif