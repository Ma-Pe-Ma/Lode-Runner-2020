#ifndef STATE_H
#define STATE_H

class StateContext;
class RenderingManager;
#include <memory>

class State {
protected:
	float timer = 0;
	std::shared_ptr<RenderingManager> renderingManager;

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

	virtual void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager)
	{
		this->renderingManager = renderingManager;
	}
};

#endif