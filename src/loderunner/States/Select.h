#ifndef SELECT_H
#define SELECT_H

#include "State.h"

class Select : public State {
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif