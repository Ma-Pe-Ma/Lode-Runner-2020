#ifndef DRAWING_H
#define DRAWING_H

#include <bitset>

#ifdef ANDROID_VERSION

#else
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include "Enums/Direction.h"
#include "Gold.h"
#include "Audio.h"

#include "GLHelper.h"

void playerTextureMapping(int);
void levelTextureMapping(int);
void abcTextureMapping(char);
void TextWriting(std::string, float, float);
void DrawLevel(float, float, int);
void DrawEnemy(float, float, int, Direction, bool);

#endif // !FUNCTIONS_H