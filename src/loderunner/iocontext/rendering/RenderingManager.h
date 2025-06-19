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
#include <map>

#include "gameplay/Brick.h"
#include "gameplay/Trapdoor.h"
#include "gameplay/Enemy.h"
#include "gameplay/Gold.h"

#include "Text.h"

#include "iocontext/IOContext.h"

#define USE_DYNAMIC_ARRAY

class RenderingManager {
protected:

	std::shared_ptr<IOContext> ioContext;

	const float levelVertices[16] = {
		-1.0f,			-1.0f,
		-14.0f / 15,	-1.0f,
		-14.0f / 15,	-8.0f / 9,
		-1.0f,			-8.0f / 9,

		0.0f,			5.0f / 6,
		1.0f / 11,		5.0f / 6,
		1.0f / 11,		1.0f,
		0.0f,			1.0f
	};

	const float enemyVertices[16] = {
		-1.0f,			-1.0f,
		-14.0f / 15,	-1.0f,
		-14.0f / 15,	-8.0f / 9,
		-1.0f,			-8.0f / 9,

		0.0f,			0.0f / 6,
		1.0f / 12,		0.0f / 6,
		1.0f / 12,		1.0f / 6,
		0.0f,			1.0f / 6
	};

	const float characterVertices[16] = {
		-96.0f / 140,	26.0f / 28,
		-90.0f / 140,	26.0f / 28,
		-90.0f / 140,	28.0f / 28,
		-96.0f / 140,	28.0f / 28,

		0.0f,			0.75f,
		1.0f / 16,		0.75f,
		1.0f / 16,		1.0f - 0.001f,
		0.0f,			1.0f - 0.001f,
	};

	const float mainVertices[16] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f, 1.0f,
		-1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	const unsigned int indices[6] = {
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

#ifdef USE_DYNAMIC_ARRAY
	int* levelTextureIDs = nullptr;
	int* levelDrawables = nullptr;
#else
	int levelTextureIDs[540];
	int levelDrawables[540 * 2];
#endif //  USE_DYNAMIC_ARRAY
	
	short levelDrawableSize = 0;
	short currentLevelDrawableSize = 0;

	std::vector<std::shared_ptr<Brick>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;
	std::vector<std::shared_ptr<Gold>> goldList;
	std::vector<std::shared_ptr<Enemy>> enemyList;

	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;
	std::vector<std::tuple<int, int>> finishingLadderList;	

	short goldStartIndex = 0;
	short goldSize = 0;

	short ladderStartIndex = 0;
	short ladderSize = 0;
	short finishingLadderSize = 0;

#ifdef USE_DYNAMIC_ARRAY
	float* enemyDrawables = nullptr;
	int* enemyTextureIDs = nullptr;
	int* enemyDirections = nullptr;
	int* enemyGoldIndicator = nullptr;
#else
	float enemyDrawables[340 * 2];
	int enemyTextureIDs[340];
	int enemyDirections[340];
	int enemyGoldIndicator[340];
#endif
	short enemyDrawableSize = 0;

	std::vector<std::shared_ptr<Text>> textList;
	
#ifdef USE_DYNAMIC_ARRAY
	int* characterTextureIDs = nullptr;
	float* characterDrawables = nullptr;
#else
	int characterTextureIDs[864];
	float characterDrawables[864 * 2];
#endif // USE_DYNAMIC_ARRAY
	
	short characterDrawableSize = 0;

	int* generatorDrawables;
	int* generatorTextures;

	void initializeBufferObjects(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, const float* vertices);

	std::string assetFolder;

public:
	RenderingManager(std::string assetFolder, std::shared_ptr<IOContext> ioContext);

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

	virtual void createShaders();
	virtual void initializeLevelLayout();
	virtual void initializeEnemies();
	virtual void initializeCharacters();

	void setupMainMenuRenderer();
	void setLadderFlashFactor(int factor);
	virtual void enableFinishingLadderDrawing();
	void setGeneratorParameters(int* generatorDrawables, int* generatorTextures);

	virtual void render();	
	virtual void renderGenerator();
	virtual void renderMainMenu(int menuCursor, int gameVersion);

	virtual void clearRenderableObjects();
	virtual void terminate();

	template<typename T>
	void checkAndDeleteArray(T*& array)
	{
		if (array != nullptr)
		{
			delete[] array;
			array = nullptr;
		}
	}

	void loadTexture(unsigned int textureID, unsigned int& textureContainer, std::string path);

	int* getPointerToDebrisTexture() { return &this->levelTextureIDs[0]; }
	int* getPointerToDebrisLocation() { return &this->levelDrawables[0]; }

	std::tuple<float*, int*> setFruitTextureID(int texture);
};

#endif