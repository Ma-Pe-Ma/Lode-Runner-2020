#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#else 
#include <glad/glad.h>
#endif

#include "Shader.h"
#include <memory>
#include <vector>

#include "../Brick.h"
#include "../Trapdoor.h"
#include "../Enemy.h"
#include "../Gold.h"

#include <map>

#include "Text.h"
#include "GLHelper.h"

class RenderingManager {
	float levelVertices[16] = {
		-1.0f,			-1.0f,
		-14.0f / 15,	-1.0f,
		-14.0f / 15,	-8.0f / 9,
		-1.0f,			-8.0f / 9,

		0.0f,			5.0f / 6,
		1.0f / 11,		5.0f / 6,
		1.0f / 11,		1.0f,
		0.0f,			1.0f
	};

	float enemyVertices[16] = {
		-1.0f,			-1.0f,
		-14.0f / 15,	-1.0f,
		-14.0f / 15,	-8.0f / 9,
		-1.0f,			-8.0f / 9,

		0.0f,			0.0f / 5,
		1.0f / 12,		0.0f / 5,
		1.0f / 12,		1.0f / 5,
		0.0f,			1.0f / 5
	};

	float characterVertices[16] = {
		-96.0f / 140,	26.0f / 28,
		-90.0f / 140,	26.0f / 28,
		-90.0f / 140,	28.0f / 28,
		-96.0f / 140,	28.0f / 28,

		0.0f,			0.75f,
		1.0f / 16,		0.75f,
		1.0f / 16,		1.0f - 0.001f,
		0.0f,			1.0f - 0.001f,
	};

	float mainVertices[16] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f, 1.0f,
		-1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	unsigned int indices[6] = {
		0,1,2,
		0,2,3
	};

	unsigned int levelVAO, levelVBO, levelEBO;
	unsigned int enemyVAO, enemyVBO, enemyEBO;
	unsigned int characterVAO, characterVBO, characterEBO;
	unsigned int mainVAO, mainVBO, mainEBO;
	unsigned int cursorVAO, cursorVBO, cursorEBO;

	unsigned int levelTexture, enemyTexture, characterTexture, originalMainTexture, championshipMainTexture;

	Shader* levelShader;
	Shader* enemyShader;
	Shader* characterShader;
	Shader* mainShader;

	int* textureIDs = nullptr;
	int* drawables = nullptr;
	int drawableSize = 0;
	int currentDrawableSize = 0;

	std::shared_ptr<std::vector<std::shared_ptr<Brick>>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;
	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;
	std::vector<std::tuple<int, int>> finishingLadderList;
	std::vector<std::shared_ptr<Gold>> goldList;

	int goldStartIndex = 0;
	int goldSize = 0;

	int ladderStartIndex = 0;
	int ladderSize = 0;
	int finishingLadderSize = 0;

	std::vector<std::shared_ptr<Enemy>> enemyList;

	int* enemyTextureIDs = nullptr;
	int* enemyDirections = nullptr;
	float* enemyDrawables = nullptr;
	int* enemyGoldIndicator = nullptr;
	int enemyDrawableSize = 0;

	std::vector<std::shared_ptr<Text>> textList;
	
	int* characterTextureIDs = nullptr;
	float* characterDrawables = nullptr;
	int characterDrawableSize = 0;

	void initializeBufferObjects(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float* vertices)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0 * sizeof(float)));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	std::string assetFolder;

public:
	RenderingManager(std::string assetFolder) {
		Text::initCharMap();
		this->assetFolder = assetFolder;

		glViewport(GLHelper::viewPortX, GLHelper::viewPortY, GLHelper::viewPortWidth, GLHelper::viewPortHeight);

		glDepthFunc(GL_LEQUAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		levelShader = new Shader(assetFolder + "Shaders/level.vs", assetFolder + "Shaders/level.fs");
		initializeBufferObjects(levelVAO, levelVBO, levelEBO, levelVertices);

		enemyShader = new Shader(assetFolder + "Shaders/player.vs", assetFolder + "Shaders/player.fs");
		initializeBufferObjects(enemyVAO, enemyVBO, enemyEBO, enemyVertices);

		characterShader = new Shader(assetFolder + "/Shaders/character.vs", assetFolder + "Shaders/character.fs");
		initializeBufferObjects(characterVAO, characterVBO, characterEBO, characterVertices);

		mainShader = new Shader(assetFolder + "/Shaders/main.vs", assetFolder + "/Shaders/main.fs");
		initializeBufferObjects(mainVAO, mainVBO, mainEBO, mainVertices);

		loadTexture(0, characterTexture, assetFolder + "Texture/NES - Lode Runner - Characters.png", true);
		loadTexture(1, levelTexture, assetFolder + "Texture/NES - Lode Runner - Tileset.png", true);
		loadTexture(2, characterTexture, assetFolder + "Texture/ABC.png");
		loadTexture(3, originalMainTexture, assetFolder + "Texture/MainMenu.png");
		loadTexture(4, championshipMainTexture, assetFolder + "Texture//Championship.png");

		mainShader->use();

		float vertexScales[4] = {24.0f / 35, 1.0f, 1.5f / 70, 0.5f / 14};
		float vertexOffsets[4] = { 0.0f, 0.0f, -13.5f / 70, 0.5f / 14 };

		mainShader->setVec2Array("vertexScales", vertexScales, 2);
		mainShader->setVec2Array("vertexOffsets", vertexOffsets, 2);

		float textureScales[4] = { 1.0f, 1.0f, 1.0f / 22, 1.0f / 12 };
		float textureOffsets[4] = { 0.0f, 0.0f, 21.0f / 22, 0.0f };

		mainShader->setVec2Array("textureScales", textureScales, 2);
		mainShader->setVec2Array("textureOffsets", textureOffsets, 2);

		float cursorOffsets[4] = { 0.0f, 0.0f, 0.0f, 2.0f / 14 };
		mainShader->setVec2Array("cursorOffsets", cursorOffsets, 2);
	}

	void setBrickList(std::shared_ptr<std::vector<std::shared_ptr<Brick>>> brickList)
	{
		this->brickList = brickList;
	}

	void setPoleList(std::vector<std::tuple<int, int>> poleList)
	{
		this->poleList = poleList;
	}

	void setConcreteList(std::vector<std::tuple<int, int>> concreteList)
	{
		this->concreteList = concreteList;
	}

	void setTrapdoorList(std::vector<std::shared_ptr<Trapdoor>> trapdoorList)
	{
		this->trapdoorList = trapdoorList;
	}

	void setLadderList(std::vector<std::tuple<int, int>> ladderList)
	{
		this->ladderList = ladderList;
	}

	void setFinishingLadderList(std::vector<std::tuple<int, int>> finishingLadderList)
	{
		this->finishingLadderList = finishingLadderList;
	}

	void setGoldList(std::vector<std::shared_ptr<Gold>> goldList)
	{
		this->goldList = goldList;
	}

	void setEnemyList(std::vector<std::shared_ptr<Enemy>> enemyList)
	{
		this->enemyList = enemyList;
	}

	void setTextList(std::vector<std::shared_ptr<Text>> textList)
	{
		this->textList = textList;
	}

	void initializeLevelLayout()
	{
		drawableSize = 1 + poleList.size() + concreteList.size() + brickList->size() + trapdoorList.size() + goldList.size() + ladderList.size() + finishingLadderList.size();
		currentDrawableSize = drawableSize - finishingLadderList.size();
		drawables = new int[drawableSize * 2];
		textureIDs = new int[drawableSize];

		Brick::setPointerToDebrisTexture(&textureIDs[0]);
		Brick::setPointerToDebrisLocation(&drawables[0]);

		for (auto iterator = brickList->begin(); iterator != brickList->end(); iterator++)
		{
			int index = iterator - brickList->begin() + 1;

			drawables[2 * index + 0] = (*iterator)->getPosition().x;
			drawables[2 * index + 1] = (*iterator)->getPosition().y;
		
			(*iterator)->setTexturePointer(&textureIDs[index]);
		}

		std::fill_n(textureIDs + 1, brickList->size(), 0);

		for (auto iterator = poleList.begin(); iterator != poleList.end(); iterator++)
		{
			int index = iterator - poleList.begin() + brickList->size() + 1;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(textureIDs + brickList->size() + 1, poleList.size(), 18);

		for (auto iterator = concreteList.begin(); iterator != concreteList.end(); iterator++)
		{
			int index = iterator - concreteList.begin() + brickList->size() + poleList.size() + 1;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(textureIDs + brickList->size() + poleList.size() + 1, concreteList.size(), 6);

		for (auto iterator = trapdoorList.begin(); iterator != trapdoorList.end(); iterator++)
		{
			int index = iterator - trapdoorList.begin() + brickList->size() + poleList.size() + concreteList.size() + 1;

			drawables[2 * index + 0] = (*iterator)->getPos().x;
			drawables[2 * index + 1] = (*iterator)->getPos().y;
			
			(*iterator)->setTexturePointer(&textureIDs[index]);
		}

		std::fill_n(textureIDs + brickList->size() + poleList.size() + concreteList.size() + 1, trapdoorList.size(), 0);

		goldStartIndex = brickList->size() + poleList.size() + concreteList.size() + trapdoorList.size() + 1;
		goldSize = goldList.size();

		for (auto iterator = goldList.begin(); iterator != goldList.end(); iterator++)
		{
			int index = iterator - goldList.begin() + goldStartIndex;

			(*iterator)->setPositionPointer(&drawables[2 * index + 0]);
			drawables[2 * index + 0] = (*iterator)->getPos().x;
			drawables[2 * index + 1] = (*iterator)->getPos().y;
		}

		std::fill_n(textureIDs + goldStartIndex, goldList.size(), 36);

		ladderStartIndex = brickList->size() + poleList.size() + concreteList.size() + trapdoorList.size() + goldList.size() + 1;
		ladderSize = ladderList.size();

		for (auto iterator = ladderList.begin(); iterator != ladderList.end(); iterator++)
		{
			int index = iterator - ladderList.begin() + brickList->size() + poleList.size() + concreteList.size() + trapdoorList.size() + goldList.size() + 1;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(textureIDs + ladderStartIndex, ladderSize, 12);

		for (auto iterator = finishingLadderList.begin(); iterator != finishingLadderList.end(); iterator++)
		{
			int index = iterator - finishingLadderList.begin() + ladderStartIndex + ladderSize;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		finishingLadderSize = finishingLadderList.size();

		brickList = nullptr;
		poleList.clear();
		concreteList.clear();
		trapdoorList.clear();
		goldList.clear();
		ladderList.clear();
		finishingLadderList.clear();

		levelShader->use();
		levelShader->setInt("textureA", 1);
	}

	void setLadderFlashFactor(int factor)
	{
		std::fill_n(textureIDs + goldStartIndex, goldSize, 36 + factor);
		std::fill_n(textureIDs + ladderStartIndex, ladderSize, 12 + factor);
	}

	void enableFinishingLadderDrawing()
	{
		currentDrawableSize = drawableSize;
		ladderSize += finishingLadderSize;
	}

	void initializeEnemies()
	{
		enemyDrawableSize = enemyList.size();
		enemyDrawables = new float[2 * enemyDrawableSize];
		enemyTextureIDs = new int[enemyDrawableSize];
		enemyDirections = new int[enemyDrawableSize];
		enemyGoldIndicator = new int[enemyDrawableSize];

		for (auto iterator = enemyList.begin(); iterator != enemyList.end(); iterator++)
		{
			int index = iterator - enemyList.begin();

			auto enemy = *iterator;

			enemy->setTexturePointer(&enemyTextureIDs[index]);
			enemy->setPositionPointer(&enemyDrawables[2 * index + 0]);
			enemy->setDirectionPointer(&enemyDirections[index]);
			enemy->setCarryGoldPointer(&enemyGoldIndicator[index]);

			enemyDrawables[2 * index + 0] = enemy->pos.x;
			enemyDrawables[2 * index + 1] = enemy->pos.y;

			enemyGoldIndicator[index] = false;
			enemyDirections[index] = false;

			enemyTextureIDs[index] = enemy->getTextureRef();
		}

		enemyList.clear();

		enemyShader->use();
		enemyShader->setInt("textureA", 0);
	}

	void initializeCharacters()
	{
		characterDrawableSize = 0;

		for (auto text : textList)
		{
			characterDrawableSize += text->getLength();
		}

		characterDrawables = new float[2 * characterDrawableSize];
		characterTextureIDs = new int[characterDrawableSize];

		int positionCounter = 0;

		for (auto text : textList)
		{
			text->setPositionPointer(&characterDrawables[2 * positionCounter]);
			text->setTexturePointer(&characterTextureIDs[positionCounter]);

			text->initPointers();

			positionCounter += text->getLength();
		}

		textList.clear();

		characterShader->use();
		characterShader->setInt("textureA", 2);
	}

	void render()
	{
		levelShader->use();
		glBindVertexArray(levelVAO);
		levelShader->setIntArray("textureID", textureIDs, currentDrawableSize);
		levelShader->setInt2Array("gPos", drawables, currentDrawableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, currentDrawableSize);

		enemyShader->use();
		glBindVertexArray(enemyVAO);
		enemyShader->setIntArray("textureID", enemyTextureIDs, enemyDrawableSize);
		enemyShader->setIntArray("carryGold", enemyGoldIndicator, enemyDrawableSize);
		enemyShader->setVec2Array("gPos", enemyDrawables, enemyDrawableSize);
		enemyShader->setIntArray("direction", enemyDirections, enemyDrawableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, enemyDrawableSize);

		characterShader->use();
		glBindVertexArray(characterVAO);
		characterShader->setIntArray("textureID", characterTextureIDs, characterDrawableSize);
		characterShader->setVec2Array("gPos", characterDrawables, characterDrawableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, characterDrawableSize);
	}

	void clearRenderableObjects()
	{
		ladderSize = 0;
		finishingLadderSize = 0;
		goldSize = 0;

		drawableSize = 0;
		currentDrawableSize = 0;
		enemyDrawableSize = 0;

		characterDrawableSize = 0;

		checkAndDeleteArray(drawables);
		checkAndDeleteArray(textureIDs);
		checkAndDeleteArray(enemyTextureIDs);
		checkAndDeleteArray(enemyDirections);
		checkAndDeleteArray(enemyDrawables);
		checkAndDeleteArray(enemyGoldIndicator);
		checkAndDeleteArray(characterTextureIDs);
		checkAndDeleteArray(characterDrawables);
	}

	template<typename T>
	void checkAndDeleteArray(T*& array)
	{
		if (array != nullptr)
		{
			delete[] array;
			array = nullptr;
		}
	}

	void loadTexture(unsigned int textureID, unsigned int& textureContainer, std::string path, bool ignoreBlack = false)
	{
		glActiveTexture(GL_TEXTURE0 + textureID);
		textureContainer = GLHelper::loadTexture(path.c_str(), ignoreBlack);
		glBindTexture(GL_TEXTURE_2D, textureContainer);
	}

	void drawMainMenu(int menuCursor, int gameVersion)
	{
		//Drawing main menu
		if (gameVersion == 1) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else {
			glClearColor(110.0 / 256, 93.0 / 256, 243.0 / 256, 1.0f);
		}

		//drawing background
		glClear(GL_COLOR_BUFFER_BIT);

		mainShader->use();
		glBindVertexArray(mainVAO);

		int mainTextures[2] = { 3 + (gameVersion == 1), 1 };		
		mainShader->setIntArray("textures", mainTextures, 2);

		int cursor[2] = { 0, -menuCursor};		
		mainShader->setIntArray("cursor", cursor, 2);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1 + (gameVersion == 0));
	}

	void terminate()
	{
		glDeleteVertexArrays(1, &levelVAO);
		glDeleteBuffers(1, &levelVBO);
		glDeleteBuffers(1, &levelEBO);

		glDeleteVertexArrays(1, &enemyVAO);
		glDeleteBuffers(1, &enemyVBO);
		glDeleteBuffers(1, &enemyEBO);

		glDeleteVertexArrays(1, &enemyVAO);
		glDeleteBuffers(1, &enemyVBO);
		glDeleteBuffers(1, &enemyEBO);

		glDeleteVertexArrays(1, &mainVAO);
		glDeleteBuffers(1, &mainVBO);
		glDeleteBuffers(1, &mainEBO);

		glDeleteVertexArrays(1, &characterVAO);
		glDeleteBuffers(1, &characterVBO);
		glDeleteBuffers(1, &characterVBO);

		glDeleteTextures(1, &levelTexture);
		glDeleteTextures(1, &enemyTexture);
		glDeleteTextures(1, &characterTexture);
		glDeleteTextures(1, &originalMainTexture);
		glDeleteTextures(1, &championshipMainTexture);

		delete levelShader;
		delete enemyShader;
		delete characterShader;
		delete mainShader;
	}
};

#endif