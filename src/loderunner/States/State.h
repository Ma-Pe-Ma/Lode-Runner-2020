#ifndef STATE_H
#define STATE_H

class StateContext;

class RenderingManager;
class IOContext;
class GameConfiguration;
class Audio;

#include <memory>

class State {
protected:
	float timer = 0;

	StateContext* stateContext;
public:	

	virtual void start() = 0;
	virtual void update(float) = 0;
	virtual void end() = 0;

	void setStateContext(StateContext*);

	StateContext* getStateContext()
	{
		return this->stateContext;
	}
};

#endif