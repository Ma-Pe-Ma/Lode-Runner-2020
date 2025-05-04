#include "Text.h"

std::map<char, int> Text::characterMap;

void Text::initCharMap()
{
	if (characterMap.empty())
	{
		characterMap['.'] = 0;
		characterMap[':'] = 1;
		characterMap[' '] = 2;
		//copyright sign
		characterMap[169] = 32;

		//adding alphabet characters
		for (int i = 0; i < 26; i++)
		{
			characterMap['A' + i] = 33 + i;
		}

		//adding number characters
		for (int i = 0; i < 10; i++)
		{
			characterMap['0' + i] = 16 + i;
		}
	}	
}

Text::Text(std::string text, Vector2D pos)
{
	this->text = text;
	this->pos = pos;

	fixedLength = text.size();
}

Text::Text(std::tuple<std::string, float, float> text) : Text(std::get<0>(text), {std::get<1>(text), std::get<2>(text) }) 
{
	
}

void Text::setPositionPointer(float* positionPointer)
{
	this->positionPointer = positionPointer;
}

void Text::setTexturePointer(int* texturePointer)
{
	this->texturePointer = texturePointer;
}

void Text::initPointers()
{
	if (text.length() != fixedLength)
	{
		text.resize(fixedLength, ' ');
	}

	for (auto iterator = text.begin(); iterator != text.end(); iterator++)
	{
		int index = iterator - text.begin();

		//int xPos = (int(pos.x) + index) % 16;
		//int yPos = pos.y + (int(pos.x) % 16 + index) / 16;

		float xPos = pos.x + index;
		float yPos = pos.y;

		positionPointer[2 * index + 0] = xPos;
		positionPointer[2 * index + 1] = yPos;

		texturePointer[index] = characterMap[*iterator];
	}
}

void Text::changeContent(std::string newText)
{
	text = newText;
	initPointers();
}

int Text::getLength()
{
	return this->text.size();
}