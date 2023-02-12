#ifndef DRAWING_H
#define DRAWING_H

#include <bitset>

#ifdef ANDROID_VERSION

#else
#if defined __EMSCRIPTEN__
	#include <GLES3/gl3.h>
	#include <emscripten/emscripten.h>
#else 
	#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#endif

#include "Enums/Direction.h"
#include "Gold.h"
#include "Audio.h"

#include "GLHelper.h"

namespace Drawing{
	extern float block[16];
	extern float main_menu[16];
	extern const unsigned int indices[6];

	void playerTextureMapping(int);
	void levelTextureMapping(int);
	void abcTextureMapping(char);
	void textWriting(std::string, float, float);
	//void drawLevel(float, float, int);
	void drawEnemy(float, float, int, Direction, bool);
}

#endif // !FUNCTIONS_H