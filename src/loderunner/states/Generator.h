#ifndef GENERATOR_H
#define GENERATOR_H

#include "State.h"
#include <map>

#include <json.hpp>

#include "gamestates/Play.h"

#include "gameplay/LayoutBlock.h"
#include "gameplay/Brick.h"
#include "gameplay/Trapdoor.h"

#include "iocontext/rendering/RenderingManager.h"

#include "iocontext/GeneratorGUI.h"

class Generator : public State {
private:
	std::array<std::array<short, 28>, 16> gen;
	std::array<std::array<int, 18>, 30> texture;
	std::array<std::array<std::array<int, 2>, 18>, 30> pos;	

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
	GeneratorGUI generatorGUI = GeneratorGUI(this);
public:
	Generator();
	void start() override;
	void update() override;
	void end() override;
	
	void setGameContext(std::shared_ptr<GameContext> gameContext)
	{
		this->gameContext = gameContext;
	}

	std::array<std::array<short, 28>, 16> getGeneratedLayout() { return this->gen; }
	void setGeneratedLayout(std::array<std::array<short, 28>, 16> newLayout = {}) {
		gen = newLayout;

		for (int j = 0; j < 16; j++) {
			for (int i = 0; i < 28; i++) {
				texture[i + 1][j + 1] = textureMap.at(gen[j][i]);
			}
		}		
	}
};

#endif