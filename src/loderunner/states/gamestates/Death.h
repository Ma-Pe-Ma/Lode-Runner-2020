#ifndef DEATH_H
#define DEATH_H

#include "GameState.h"

class Death : public GameState {
public:
	void start() override;
	void update() override;
	void end() override;
};

#endif