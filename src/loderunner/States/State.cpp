#include "States/State.h"
#include "States/StateContext.h"

void State::setStateContext(StateContext* stateContext) {
	this->stateContext = stateContext;
}