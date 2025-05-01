#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <map>

#include "gameplay/Vector2D.h"

class Text {
	std::string text;

	float* positionPointer;
	int* texturePointer;

	Vector2D pos;

	int fixedLength = 0;

	static std::map<char, int> characterMap;

public:
	static void initCharMap();

	Text(std::string text, Vector2D pos);
	Text(std::tuple<std::string, float, float>);
	void setPositionPointer(float* positionPointer);

	void setTexturePointer(int* texturePointer);

	void initPointers();

	int getLength();

	void changeContent(std::string newText);
};

#endif // !TEXT_H
