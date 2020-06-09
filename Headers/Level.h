#ifndef LEVEL_H
#define LEVEL_H

#include "variables.h"
#include "Enemy.h"
#include "Gold.h"

#include <fstream>
#include <string>

void LevelLoading(unsigned int levelNumber);
void HighestLadder();
void HighestLadderGen();

#endif // !LEVEL_H