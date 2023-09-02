#ifndef SELECT_H
#define SELECT_H

#include "State.h"

#include "iocontext/rendering/RenderingManager.h"
#include <string>

class Select : public State {
	void changeLevelNumber(int&);

	const std::shared_ptr<Text> levelText = std::make_shared<Text>(Text("STAGE    ", { 8,12 }));
public:
	void start() override;
	void update() override;
	void end() override;
};

#endif