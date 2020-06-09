#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "Enums/Menu.h"
#include "Gold.h"
#include "Enemy.h"
#include "Functions.h"
#include "Level.h"

void mainMenu(float currentFrame);
void introScreen(float currentFrame);
void selectScreen(float currentFrame);
void gameScreen(float currentFrame);
void pauseScreen(float currentFrame);
void pauseScreenOut(float currentFrame);
void outroScreen(float currentFrame);
void gameOverScreen(float currentFrame);
void generatorScreen(float currentFrame);

#endif // !STRUCTURE_H