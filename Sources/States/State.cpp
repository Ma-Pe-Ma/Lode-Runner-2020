#include "States/State.h"
#include "States/StateContext.h"

StateContext* State::stateContext;

void State::initialize(StateContext* stateContext) {
	State::stateContext = stateContext;
}

void State::setStateContext(StateContext* stateContext) {
	this->stateContext = stateContext;
}