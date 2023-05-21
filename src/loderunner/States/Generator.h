#ifndef GENERATOR_H
#define GENERATOR_H

#include "State.h"
#include <map>
#include "Enums/LayoutBlock.h"
#include "Brick.h"
#include "Trapdoor.h"
#include "GameStates/Play.h"

#include "Rendering/RenderingManager.h"

class Generator : public State {
private:
	short gen[30][18];
	
	int pos[30][18][2];
	int texture[30][18];

	short geX = 1;
	short geY = 16;
	std::map<int, int> textureMap;
	std::map<int, LayoutBlock> layoutMap;

	void generateLevel();

	std::shared_ptr<GameContext> gameContext;

	std::shared_ptr<Text> timeText;
public:
	Generator();
	void start() override;
	void update(float) override;
	void end() override;
	
	void setGameContext(std::shared_ptr<GameContext> gameContext)
	{
		this->gameContext = gameContext;
	}
};

#endif