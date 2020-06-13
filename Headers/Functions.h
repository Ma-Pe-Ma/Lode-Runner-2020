#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <bitset>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Enums/Direction.h"
#include "variables.h"
#include "Gold.h"
#include "Audio.h"

#include "GLHelper.h"

void playerTextureMapping(int index);
void levelTextureMapping(int index);
void abcTextureMapping(char karakter);
void TextWriting(std::string text, float i, float j);
void DrawLevel(float x, float y, int ref, float holeTimer);
void DrawEnemy(float x, float y, int ref, Direction direction, Gold* gold);
void processInput(GLFWwindow* window);
void loadConfig();

#endif // !FUNCTIONS_H