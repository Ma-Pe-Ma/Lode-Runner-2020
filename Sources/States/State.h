#ifndef STATE_H
#define STATE_H

class StateContext;

class State {
protected:
	float timer = 0;

public:
	static StateContext* stateContext;
	static void initialize(StateContext*);

	virtual void start() = 0;
	virtual void update(float) = 0;
	virtual void end() = 0;

	void SetStateContext(StateContext*);
};

#endif