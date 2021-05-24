#ifndef PAUSE_H
#define PAUSE_H

#include "GameState.h"

class Pause : public GameState {
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif