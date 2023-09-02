#ifndef GENERATOR_H
#define GENERATOR_H

#include "State.h"
#include <map>

#include "gameStates/Play.h"

#include "gameplay/LayoutBlock.h"
#include "gameplay/Brick.h"
#include "gameplay/Trapdoor.h"

#include "iocontext/rendering/RenderingManager.h"

class Generator : public State {
private:
	short gen[30][18];
	
	int pos[30][18][2];
	int texture[30][18];

	short geX = 1;
	short geY = 16;
	const std::map<int, int> textureMap = {
		{0, 17},
		{1, 0},
		{2, 6},
		{3, 12},
		{4, 18},
		{5, 24},
		{6, 30},
		{7, 36},
		{8, 42},
		{9, 48},
	};

	std::shared_ptr<GameContext> gameContext;
public:
	Generator();
	void start() override;
	void update() override;
	void end() override;
	
	void setGameContext(std::shared_ptr<GameContext> gameContext)
	{
		this->gameContext = gameContext;
	}
};

#endif