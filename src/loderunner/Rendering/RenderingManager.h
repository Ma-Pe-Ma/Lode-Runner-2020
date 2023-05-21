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
#include <tuple>

#include "../Brick.h"
#include "../Trapdoor.h"
#include "../Enemy.h"
#include "../Gold.h"

#include <map>

#include "Text.h"

#include "../iocontext/IOContext.h"
#

class GameContext;

#define USE_DYNAMIC_ARRAY

class RenderingManager {
	std::shared_ptr<GameContext> gameContext;
	std::shared_ptr<IOContext> ioContext;

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

	unsigned int levelTexture, enemyTexture, characterTexture, originalMainTexture, championshipMainTexture;

	Shader* levelShader;
	Shader* enemyShader;
	Shader* characterShader;
	Shader* mainShader;

#ifdef  USE_DYNAMIC_ARRAY
	int* levelTextureIDs = nullptr;
	int* levelDrawables = nullptr;
#else
	int levelTextureIDs[540];
	int levelDrawables[540 * 2];
#endif //  USE_DYNAMIC_ARRAY
	
	int levelDrawableSize = 0;
	int currentLevelDrawableSize = 0;

	std::vector<std::shared_ptr<Brick>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;
	std::vector<std::shared_ptr<Gold>> goldList;
	std::vector<std::shared_ptr<Enemy>> enemyList;

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;
	std::vector<std::tuple<int, int>> finishingLadderList;	

	int goldStartIndex = 0;
	int goldSize = 0;

	int ladderStartIndex = 0;
	int ladderSize = 0;
	int finishingLadderSize = 0;

#ifdef  USE_DYNAMIC_ARRAY
	float* enemyDrawables = nullptr;
	int* enemyTextureIDs = nullptr;
	int* enemyDirections = nullptr;
	int* enemyGoldIndicator = nullptr;
#else
	float enemyDrawables[540 * 2];
	int enemyTextureIDs[340];
	int enemyDirections[340];
	int enemyGoldIndicator[340];
#endif

	int enemyDrawableSize = 0;

	std::vector<std::shared_ptr<Text>> textList;
	
#ifdef USE_DYNAMIC_ARRAY
	int* characterTextureIDs = nullptr;
	float* characterDrawables = nullptr;
#else
	int characterTextureIDs[864];
	float characterDrawables[864 * 2];
#endif // USE_DYNAMIC_ARRAY
	
	int characterDrawableSize = 0;

	int* generatorDrawables;
	int* generatorTextures;

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
	RenderingManager(std::string assetFolder, std::string mainCover, std::shared_ptr<IOContext> ioContext) {
		Text::initCharMap();
		this->assetFolder = assetFolder;
		this->ioContext = ioContext;

		glViewport(std::get<0>(ioContext->getViewPortPosition()), std::get<1>(ioContext->getViewPortPosition()), std::get<0>(ioContext->getViewPortSize()), std::get<1>(ioContext->getViewPortSize()));

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

		loadTexture(0, enemyTexture, assetFolder + "Texture/NES - Lode Runner - Characters.png", true);
		loadTexture(1, levelTexture, assetFolder + "Texture/NES - Lode Runner - Tileset.png", true);
		loadTexture(2, characterTexture, assetFolder + "Texture/ABC.png");
		loadTexture(3, originalMainTexture, assetFolder + mainCover);
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

	void setBrickList(std::vector<std::shared_ptr<Brick>> brickList)
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
		levelDrawableSize = 1 + poleList.size() + concreteList.size() + brickList.size() + trapdoorList.size() + goldList.size() + ladderList.size() + finishingLadderList.size();
		currentLevelDrawableSize = levelDrawableSize - finishingLadderList.size();
		
#ifdef USE_DYNAMIC_ARRAY
		levelDrawables = new int[levelDrawableSize * 2];
		levelTextureIDs = new int[levelDrawableSize];
#endif // USE_DYNAMIC_ARRAY		

		gameContext->setPointerToDebrisTexture(&levelTextureIDs[0]);
		gameContext->setPointerToDebrisLocation(&levelDrawables[0]);

		for (auto iterator = brickList.begin(); iterator != brickList.end(); iterator++)
		{
			int index = iterator - brickList.begin() + 1;

			levelDrawables[2 * index + 0] = (*iterator)->getPosition().x;
			levelDrawables[2 * index + 1] = (*iterator)->getPosition().y;
		
			(*iterator)->setTexturePointer(&levelTextureIDs[index]);
		}

		std::fill_n(levelTextureIDs + 1, brickList.size(), 0);

		for (auto iterator = poleList.begin(); iterator != poleList.end(); iterator++)
		{
			int index = iterator - poleList.begin() + brickList.size() + 1;

			levelDrawables[2 * index + 0] = std::get<0>(*iterator);
			levelDrawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(levelTextureIDs + brickList.size() + 1, poleList.size(), 18);

		for (auto iterator = concreteList.begin(); iterator != concreteList.end(); iterator++)
		{
			int index = iterator - concreteList.begin() + brickList.size() + poleList.size() + 1;

			levelDrawables[2 * index + 0] = std::get<0>(*iterator);
			levelDrawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(levelTextureIDs + brickList.size() + poleList.size() + 1, concreteList.size(), 6);

		for (auto iterator = trapdoorList.begin(); iterator != trapdoorList.end(); iterator++)
		{
			int index = iterator - trapdoorList.begin() + brickList.size() + poleList.size() + concreteList.size() + 1;

			levelDrawables[2 * index + 0] = (*iterator)->getPos().x;
			levelDrawables[2 * index + 1] = (*iterator)->getPos().y;
			
			(*iterator)->setTexturePointer(&levelTextureIDs[index]);
		}

		std::fill_n(levelTextureIDs + brickList.size() + poleList.size() + concreteList.size() + 1, trapdoorList.size(), 0);

		goldStartIndex = brickList.size() + poleList.size() + concreteList.size() + trapdoorList.size() + 1;
		goldSize = goldList.size();

		for (auto iterator = goldList.begin(); iterator != goldList.end(); iterator++)
		{
			int index = iterator - goldList.begin() + goldStartIndex;

			(*iterator)->setPositionPointer(&levelDrawables[2 * index + 0]);
			levelDrawables[2 * index + 0] = (*iterator)->getPos().x;
			levelDrawables[2 * index + 1] = (*iterator)->getPos().y;
		}

		std::fill_n(levelTextureIDs + goldStartIndex, goldList.size(), 36);

		ladderStartIndex = brickList.size() + poleList.size() + concreteList.size() + trapdoorList.size() + goldList.size() + 1;
		ladderSize = ladderList.size();

		for (auto iterator = ladderList.begin(); iterator != ladderList.end(); iterator++)
		{
			int index = iterator - ladderList.begin() + brickList.size() + poleList.size() + concreteList.size() + trapdoorList.size() + goldList.size() + 1;

			levelDrawables[2 * index + 0] = std::get<0>(*iterator);
			levelDrawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(levelTextureIDs + ladderStartIndex, ladderSize, 12);

		for (auto iterator = finishingLadderList.begin(); iterator != finishingLadderList.end(); iterator++)
		{
			int index = iterator - finishingLadderList.begin() + ladderStartIndex + ladderSize;

			levelDrawables[2 * index + 0] = std::get<0>(*iterator);
			levelDrawables[2 * index + 1] = std::get<1>(*iterator);
		}

		finishingLadderSize = finishingLadderList.size();

		levelShader->use();
		levelShader->setInt("textureA", 1);
	}

	void setLadderFlashFactor(int factor)
	{
		std::fill_n(levelTextureIDs + goldStartIndex, goldSize, 36 + factor);
		std::fill_n(levelTextureIDs + ladderStartIndex, ladderSize, 12 + factor);
	}

	void enableFinishingLadderDrawing()
	{
		currentLevelDrawableSize = levelDrawableSize;
		ladderSize += finishingLadderSize;
	}

	void initializeEnemies()
	{
		enemyDrawableSize = enemyList.size();

#ifdef USE_DYNAMIC_ARRAY
		enemyDrawables = new float[2 * enemyDrawableSize];
		enemyTextureIDs = new int[enemyDrawableSize];
		enemyDirections = new int[enemyDrawableSize];
		enemyGoldIndicator = new int[enemyDrawableSize];
#endif

		for (auto iterator = enemyList.begin(); iterator != enemyList.end(); iterator++)
		{
			int index = iterator - enemyList.begin();

			auto enemy = *iterator;

			enemy->setTexturePointer(&enemyTextureIDs[index]);
			enemy->setPositionPointer(&enemyDrawables[2 * index + 0]);
			enemy->setDirectionPointer(&enemyDirections[index]);
			enemy->setCarryGoldPointer(&enemyGoldIndicator[index]);

			enemyDrawables[2 * index + 0] = enemy->getPos().x;
			enemyDrawables[2 * index + 1] = enemy->getPos().y;

			enemyGoldIndicator[index] = false;
			enemyDirections[index] = false;

			enemyTextureIDs[index] = enemy->getTextureMap().going;
		}

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

#ifdef USE_DYNAMIC_ARRAY
		characterDrawables = new float[2 * characterDrawableSize];
		characterTextureIDs = new int[characterDrawableSize];
#endif

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

	void setGeneratorParameters(int* generatorDrawables, int* generatorTextures)
	{
		this->generatorDrawables = generatorDrawables;
		this->generatorTextures = generatorTextures;

		levelShader->use();
		levelShader->setInt("textureA", 1);
	}

	void render()
	{
		levelShader->use();
		glBindVertexArray(levelVAO);
		levelShader->setIntArray("textureID", levelTextureIDs, currentLevelDrawableSize);
		levelShader->setInt2Array("gPos", levelDrawables, currentLevelDrawableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, currentLevelDrawableSize);

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
		levelDrawableSize = 0;
		currentLevelDrawableSize = 0;

		enemyDrawableSize = 0;

		characterDrawableSize = 0;

		goldStartIndex = 0;
		goldSize = 0;

		ladderStartIndex = 0;
		ladderSize = 0;
		finishingLadderSize = 0;

#ifdef USE_DYNAMIC_ARRAY
		checkAndDeleteArray(levelDrawables);
		checkAndDeleteArray(levelTextureIDs);

		checkAndDeleteArray(enemyDrawables);
		checkAndDeleteArray(enemyTextureIDs);
		checkAndDeleteArray(enemyDirections);		
		checkAndDeleteArray(enemyGoldIndicator);

		checkAndDeleteArray(characterTextureIDs);
		checkAndDeleteArray(characterDrawables);
#endif
	
		brickList.clear();
		trapdoorList.clear();
		goldList.clear();

		enemyList.clear();

		poleList.clear();
		concreteList.clear();

		ladderList.clear();
		finishingLadderList.clear();

		textList.clear();
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
		textureContainer = ioContext->loadTexture(path.c_str());
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
		int cursor[2] = { 0, -menuCursor };
		
		mainShader->setIntArray("textures", mainTextures, 2);
		mainShader->setIntArray("cursor", cursor, 2);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1 + (gameVersion == 0));
	}

	void renderGenerator()
	{
		levelShader->use();
		glBindVertexArray(levelVAO);
		levelShader->setIntArray("textureID", generatorTextures, 540);
		levelShader->setInt2Array("gPos", generatorDrawables, 540);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 540);
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

	void setGameContext(std::shared_ptr<GameContext> gameContext)
	{
		this->gameContext = gameContext;
	}
};

#endif