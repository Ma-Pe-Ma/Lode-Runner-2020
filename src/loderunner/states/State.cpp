#include "States/State.h"
#include "States/StateContext.h"

void State::setStateContext(StateContext* stateContext) {
	this->stateContext = stateContext;
}

float State::calculateEllapsedTime()
{
	std::chrono::duration<float, std::milli> work_time = std::chrono::system_clock::now() - startTimePoint;
	return work_time.count() / 1000;
}