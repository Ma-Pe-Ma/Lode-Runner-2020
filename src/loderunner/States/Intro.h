#ifndef INTRO_H
#define INTRO_H

#include "State.h"
#include <string>

#include "Rendering/RenderingManager.h"

class Intro : public State {
private:
	std::shared_ptr<Text> levelName;
	std::shared_ptr<Text> playerName;
	std::shared_ptr<Text> lifeLeft;
	std::shared_ptr<Text> scoreText;
	std::shared_ptr<Text> hiscore;

	void saveCurrentLevel();

	void setupRenderingManager();
public:
	void start() override;
	void update(float) override;
	void end() override;
};

#endif