#ifndef GENERATOR_H
#define GENERATOR_H

#include "State.h"
#include <map>
#include "Enums/LayoutBlock.h"
#include "Brick.h"
#include "Trapdoor.h"
#include "GameStates/Play.h"
#include "IOHandler.h"

class Generator : public State {
private:
	short gen[30][18];
	short geX = 1;
	short geY = 16;
	std::map<int, int> textureMap;
	std::map<int, LayoutBlock> layoutMap;
	
	static LayoutBlock** layout;
	static std::unique_ptr<Brick>** brickO;
	static std::unique_ptr<Trapdoor>** trapdoors;
	static Play* play;

public:
	Generator();
	void start() override;
	void update(float) override;
	void end() override;
	static void setLayoutPointers(LayoutBlock**, std::unique_ptr <Brick>**, std::unique_ptr<Trapdoor>**, Play*);
};

#endif