#ifndef BEGIN_H
#define BEGIN_H

#include "GameState.h"

class Begin : public GameState {
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif // !BEGIN_H

