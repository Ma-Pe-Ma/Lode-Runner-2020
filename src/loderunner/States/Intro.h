#ifndef INTRO_H
#define INTRO_H

#include "State.h"
#include <string>

class Intro : public State {
private:
	std::string levelName;
	std::string playerName;
	std::string lifeLeft;
	std::string scoret;
	std::string hiscore;	
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif