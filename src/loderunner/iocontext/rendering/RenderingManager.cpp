#include "RenderingManager.h"

#include "gameplay/Brick.h"
#include "gameplay/Trapdoor.h"
#include "gameplay/Enemy.h"
#include "gameplay/Gold.h"

RenderingManager::RenderingManager(std::string assetFolder, std::shared_ptr<IOContext> ioContext) {
	Text::initCharMap();
	this->assetFolder = assetFolder;
	this->ioContext = ioContext;

	auto& screenParameters = ioContext->getScreenParameters();
	glViewport(std::get<0>(screenParameters.viewPortPosition), std::get<1>(screenParameters.viewPortPosition), std::get<0>(screenParameters.viewPortSize), std::get<1>(screenParameters.viewPortSize));

	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
	
	initializeBufferObjects(levelVAO, levelVBO, levelEBO, levelPositionIBO, levelTextureIBO, levelVertices);
	initializeBufferObjects(enemyVAO, enemyVBO, enemyEBO, enemyPositionIBO, enemyTextureIBO, enemyVertices);
	initializeBufferObjects(characterVAO, characterVBO, characterEBO, characterPositionIBO, characterTextureIBO, characterVertices);
	initializeBufferObjects(mainVAO, mainVBO, mainEBO, mainPositionIBO, mainTextureIBO, mainVertices);

	// initialize extra buffer for enemies
	glBindVertexArray(enemyVAO);
	glGenBuffers(1, &enemyPropertiesIBO);
	glBindBuffer(GL_ARRAY_BUFFER, enemyPropertiesIBO);
	glBufferData(GL_ARRAY_BUFFER, 30 * 18 * 2 * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribIPointer(4, 1, GL_INT, 2 * sizeof(int), (void*)(0 * sizeof(int)));
	glVertexAttribDivisor(4, 1);

	glVertexAttribIPointer(5, 1, GL_INT, 2 * sizeof(int), (void*)(1 * sizeof(int)));
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	glBindVertexArray(0);

	loadTexture(0, enemyTexture, assetFolder + "textures/NES - Lode Runner - Characters.png");
	loadTexture(1, levelTexture, assetFolder + "textures/NES - Lode Runner - Tileset.png");
	loadTexture(2, characterTexture, assetFolder + "textures/ABC.png");
	loadTexture(3, originalMainTexture, assetFolder + "textures/MainMenu.png");
	loadTexture(4, championshipMainTexture, assetFolder + "textures/Championship.png");	
}

void RenderingManager::createShaders() {
	levelShader = new Shader(assetFolder + "/shaders/level.vs", assetFolder + "/shaders/level.fs");
	enemyShader = new Shader(assetFolder + "/shaders/player.vs", assetFolder + "/shaders/player.fs");
	characterShader = new Shader(assetFolder + "/shaders/character.vs", assetFolder + "/shaders/character.fs");
	mainShader = new Shader(assetFolder + "/shaders/main.vs", assetFolder + "/shaders/main.fs");

	setupMainMenuRenderer();	
}

void RenderingManager::setupMainMenuRenderer()
{
	mainShader->use();

	float vertexScales[4] = { 24.0f / 35, 1.0f, 1.5f / 70, 0.5f / 14 };
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

void RenderingManager::initializeBufferObjects(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& positionIBO, unsigned int& textureIBO,const float* vertices)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &positionIBO);
	glGenBuffers(1, &textureIBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, positionIBO);
	glBufferData(GL_ARRAY_BUFFER, 30 * 18 * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0 * sizeof(float)));	
	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, textureIBO);
	glBufferData(GL_ARRAY_BUFFER, 30 * 18 * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribIPointer(3, 1, GL_INT, 1 * sizeof(int), (void*)(0 * sizeof(int)));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void RenderingManager::loadTexture(unsigned int textureID, unsigned int& textureContainer, std::string path)
{
	glActiveTexture(GL_TEXTURE0 + textureID);
	textureContainer = ioContext->loadTexture(path.c_str());
	glBindTexture(GL_TEXTURE_2D, textureContainer);
}

void RenderingManager::initializeLevelLayout()
{
	int brickNumber = 0;

	for (int i = 0; i < brickMap.size(); i++) {
		for (int j = 0; j < brickMap[i].size(); j++) {
			if (brickMap[i][j]) {
				brickNumber += 1;
			}
		}
	}

	levelDrawableSize = 1 + poleList.size() + concreteList.size() + brickNumber + trapdoorList.size() + goldList.size() + ladderList.size() + finishingLadderList.size();
	currentLevelDrawableSize = levelDrawableSize - finishingLadderList.size();

#ifdef USE_DYNAMIC_ARRAY
	levelDrawables = new float[levelDrawableSize * 4];
	levelTextureIDs = new int[levelDrawableSize];
#endif // USE_DYNAMIC_ARRAY		

	//draw brick debris off-screen first
	levelDrawables[0] = -1.0f;
	levelDrawables[1] = -1.0f;

	brickNumber = 0;
	for (int i = 0; i < brickMap.size(); i++) {
		for (int j = 0; j < brickMap[i].size(); j++) {
			if (brickMap[i][j]) {
				brickNumber += 1;

				levelDrawables[2 * brickNumber + 0] = i;
				levelDrawables[2 * brickNumber + 1] = j;
				brickTextureMap[i][j] = &levelTextureIDs[brickNumber];
			}
		}
	}

	std::fill_n(levelTextureIDs + 1, brickNumber, 0);

	for (auto iterator = poleList.begin(); iterator != poleList.end(); iterator++)
	{
		int index = iterator - poleList.begin() + brickNumber + 1;

		levelDrawables[2 * index + 0] = std::get<0>(*iterator);
		levelDrawables[2 * index + 1] = std::get<1>(*iterator);
	}

	std::fill_n(levelTextureIDs + brickNumber + 1, poleList.size(), 18);

	for (auto iterator = concreteList.begin(); iterator != concreteList.end(); iterator++)
	{
		int index = iterator - concreteList.begin() + brickNumber + poleList.size() + 1;

		levelDrawables[2 * index + 0] = std::get<0>(*iterator);
		levelDrawables[2 * index + 1] = std::get<1>(*iterator);
	}

	std::fill_n(levelTextureIDs + brickNumber + poleList.size() + 1, concreteList.size(), 6);

	for (auto iterator = trapdoorList.begin(); iterator != trapdoorList.end(); iterator++)
	{
		int index = iterator - trapdoorList.begin() + brickNumber + poleList.size() + concreteList.size() + 1;

		levelDrawables[2 * index + 0] = (*iterator)->getPos().x;
		levelDrawables[2 * index + 1] = (*iterator)->getPos().y;

		(*iterator)->setTexturePointer(&levelTextureIDs[index]);
	}

	std::fill_n(levelTextureIDs + brickNumber + poleList.size() + concreteList.size() + 1, trapdoorList.size(), 0);

	goldStartIndex = brickNumber + poleList.size() + concreteList.size() + trapdoorList.size() + 1;
	goldSize = goldList.size();

	for (auto iterator = goldList.begin(); iterator != goldList.end(); iterator++)
	{
		int index = iterator - goldList.begin() + goldStartIndex;

		(*iterator)->setPositionPointer(&levelDrawables[2 * index + 0]);
		levelDrawables[2 * index + 0] = (*iterator)->getPos().x;
		levelDrawables[2 * index + 1] = (*iterator)->getPos().y;
	}

	std::fill_n(levelTextureIDs + goldStartIndex, goldList.size(), 36);

	ladderStartIndex = brickNumber + poleList.size() + concreteList.size() + trapdoorList.size() + goldList.size() + 1;
	ladderSize = ladderList.size();

	for (auto iterator = ladderList.begin(); iterator != ladderList.end(); iterator++)
	{
		int index = iterator - ladderList.begin() + brickNumber + poleList.size() + concreteList.size() + trapdoorList.size() + goldList.size() + 1;

		levelDrawables[2 * index + 0] = std::get<0>(*iterator);
		levelDrawables[2 * index + 1] = std::get<1>(*iterator);
	}

	for (auto iterator = finishingLadderList.begin(); iterator != finishingLadderList.end(); iterator++)
	{
		int index = iterator - finishingLadderList.begin() + ladderStartIndex + ladderSize;

		levelDrawables[2 * index + 0] = std::get<0>(*iterator);
		levelDrawables[2 * index + 1] = std::get<1>(*iterator);
	}

	finishingLadderSize = finishingLadderList.size();

	std::fill_n(levelTextureIDs + ladderStartIndex, ladderSize + finishingLadderSize, 12);

	levelShader->use();
	levelShader->setInt("textureA", 1);
}

void RenderingManager::initializeEnemies() {
	enemyDrawableSize = enemyList.size() + 1;

#ifdef USE_DYNAMIC_ARRAY
	enemyDrawables = new float[2 * enemyDrawableSize];
	enemyTextureIDs = new int[enemyDrawableSize];
	enemyProperties = new int[2 * enemyDrawableSize];
#endif

	for (auto iterator = enemyList.begin(); iterator != enemyList.end(); iterator++)
	{
		int index = iterator - enemyList.begin();

		auto enemy = *iterator;
		enemy->setTexturePointer(&enemyTextureIDs[index]);
		enemy->setPositionPointer(&enemyDrawables[2 * index]);
		enemy->setPropertyPointer(&enemyProperties[2 * index]);
	}

	enemyShader->use();
	enemyShader->setInt("textureA", 0);
}

float* RenderingManager::setFruitTextureID(int textureID) {
	enemyTextureIDs[enemyDrawableSize - 1] = textureID;
	float* fruitPosition = &enemyDrawables[2 * (enemyDrawableSize - 1)];
	int* fruitDirection = &enemyProperties[2 * (enemyDrawableSize - 1)];

	return fruitPosition;
}

void RenderingManager::initializeCharacters()
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

void RenderingManager::setLadderFlashFactor(int factor)
{
	std::fill_n(levelTextureIDs + goldStartIndex, goldSize, 36 + factor);
	std::fill_n(levelTextureIDs + ladderStartIndex, ladderSize, 12 + factor);
}

void RenderingManager::enableFinishingLadderDrawing()
{
	currentLevelDrawableSize = levelDrawableSize;
	ladderSize += finishingLadderSize;
}

void RenderingManager::setGeneratorParameters(float* generatorDrawables, int* generatorTextures)
{
	this->generatorDrawables = generatorDrawables;
	this->generatorTextures = generatorTextures;

	levelShader->use();
	levelShader->setInt("textureA", 1);
}

void RenderingManager::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	levelShader->use();

	glBindBuffer(GL_ARRAY_BUFFER, levelPositionIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, currentLevelDrawableSize * sizeof(float) * 2, levelDrawables);

	glBindBuffer(GL_ARRAY_BUFFER, levelTextureIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, currentLevelDrawableSize * sizeof(int), levelTextureIDs);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(levelVAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, currentLevelDrawableSize);

	enemyShader->use();
	
	glBindBuffer(GL_ARRAY_BUFFER, enemyPositionIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, enemyDrawableSize * sizeof(float) * 2, enemyDrawables);

	glBindBuffer(GL_ARRAY_BUFFER, enemyTextureIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, enemyDrawableSize * sizeof(int), enemyTextureIDs);

	glBindBuffer(GL_ARRAY_BUFFER, enemyPropertiesIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, enemyDrawableSize * sizeof(int) * 2, enemyProperties);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(enemyVAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, enemyDrawableSize);

	characterShader->use();

	glBindBuffer(GL_ARRAY_BUFFER, characterPositionIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, characterDrawableSize * sizeof(float) * 2, characterDrawables);

	glBindBuffer(GL_ARRAY_BUFFER, characterTextureIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, characterDrawableSize * sizeof(int), characterTextureIDs);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(characterVAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, characterDrawableSize);
}

void RenderingManager::clearRenderableObjects()
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
	checkAndDeleteArray(enemyProperties);

	checkAndDeleteArray(characterTextureIDs);
	checkAndDeleteArray(characterDrawables);
#endif

	brickMap = { false };
	trapdoorList.clear();
	goldList.clear();

	enemyList.clear();

	poleList.clear();
	concreteList.clear();

	ladderList.clear();
	finishingLadderList.clear();

	textList.clear();
}

void RenderingManager::renderMainMenu(int menuCursor, int textureID)
{
	//Drawing main menu
	if (textureID == 4) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		glClearColor(110.0 / 256, 93.0 / 256, 243.0 / 256, 1.0f);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mainShader->use();
	glBindVertexArray(mainVAO);

	int mainTextures[2] = { textureID, 1 };
	int cursor[2] = { 0, -menuCursor };

	mainShader->setIntArray("textures", mainTextures, 2);
	mainShader->setIntArray("cursor", cursor, 2);

	if (textureID == 3) {
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 2);
	}
	else {
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
	}

	characterShader->use();
	glBindVertexArray(characterVAO);

	glBindBuffer(GL_ARRAY_BUFFER, characterPositionIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, characterDrawableSize * sizeof(float) * 2, characterDrawables);

	glBindBuffer(GL_ARRAY_BUFFER, characterTextureIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, characterDrawableSize * sizeof(int), characterTextureIDs);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, characterDrawableSize);
}

void RenderingManager::renderGenerator()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	levelShader->use();
	glBindVertexArray(levelVAO);

	glBindBuffer(GL_ARRAY_BUFFER, levelPositionIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 540 * sizeof(float) * 2, generatorDrawables);

	glBindBuffer(GL_ARRAY_BUFFER, levelTextureIBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 540 * sizeof(int), generatorTextures);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 540);
}

void RenderingManager::terminate()
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