#ifndef STATE_H
#define STATE_H

class StateContext;

class RenderingManager;
class IOContext;
class GameConfiguration;
class Audio;

#include <memory>
#include <chrono>

class State {
protected:
	StateContext* stateContext;

	std::chrono::system_clock::time_point startTimePoint;
	float calculateEllapsedTime();
public:	

	virtual void start() = 0;
	virtual void update() = 0;
	virtual void end() = 0;

	void setStateContext(StateContext*);

	StateContext* getStateContext()
	{
		return this->stateContext;
	}
};

#endif