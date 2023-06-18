#ifndef SELECT_H
#define SELECT_H

#include "State.h"

#include "Rendering/RenderingManager.h"
#include <string>

class Select : public State {
	void changeLevelNumber(int&);

	const std::shared_ptr<Text> levelText = std::make_shared<Text>(Text("STAGE    ", { 8,12 }));
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif