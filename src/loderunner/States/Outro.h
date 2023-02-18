#ifndef OUTRO_H
#define OUTRO_H

#include "State.h"
#include <string>
#include <memory>

class RenderingManager;
class Player;
class Text;

class Outro : public State {
private:
	std::shared_ptr<Text> goldPoints = nullptr;
	std::shared_ptr<Text> enemyPoints = nullptr;
	std::shared_ptr<Text> totalPoints = nullptr;

	short enemyScore = 0;
	short goldScore = 0;
	short fruitID = 0;

	std::shared_ptr<RenderingManager> renderingManager = nullptr;
	void setupRenderingManager();

	std::shared_ptr<Player> player;
public:
	void start() override;
	void update(float) override;
	void end() override;

	void setScoreParameters(short, short, short);

	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager)
	{
		this->renderingManager = renderingManager;
	}
};

#endif