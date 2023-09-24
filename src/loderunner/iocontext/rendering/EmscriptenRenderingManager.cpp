#include "EmscriptenRenderingManager.h"

#include "../EmscriptenIOContext.h"

void EmscriptenRenderingManager::createShaders() {
	maxVertexUniformNumber = EmscriptenHandler::getVertexMaxUniformNumber();
	maxFragmentUniformNumber = EmscriptenHandler::getFragmentMaxUniformNumber();

	levelShader = new Shader(assetFolder + "/Shaders/level.vs", assetFolder + "/Shaders/level.fs", std::make_format_args(maxVertexUniformNumber), std::make_format_args(maxFragmentUniformNumber));
	enemyShader = new Shader(assetFolder + "/Shaders/player.vs", assetFolder + "/Shaders/player.fs", std::make_format_args(maxVertexUniformNumber), std::make_format_args(maxFragmentUniformNumber/3));
	characterShader = new Shader(assetFolder + "/Shaders/character.vs", assetFolder + "/Shaders/character.fs", std::make_format_args(maxVertexUniformNumber), std::make_format_args(maxFragmentUniformNumber));
	mainShader = new Shader(assetFolder + "/Shaders/main.vs", assetFolder + "/Shaders/main.fs", std::make_format_args(""), std::make_format_args(""));
	
	if (EmscriptenHandler::is_mobile()) {
		touchInputShader = new Shader(assetFolder + "/Shaders/touch.vs", assetFolder + "/Shaders/touch.fs", std::make_format_args(""), std::make_format_args(""));;
		setupTouchButtonRenderer();
	}
	
	setupMainMenuRenderer();
	setUpRenderableCount(generatorRenderableCount, 540, 1);

	int width;
	int height;
	emscripten_get_canvas_element_size("canvas", &width, &height);
	emscriptenIOContext->framebufferSizeCallback(width, height);
}

void EmscriptenRenderingManager::render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int offset = 0;
	levelShader->use();
	glBindVertexArray(levelVAO);	
	for (auto renderableSize : levelRenderableCount) {
		levelShader->setIntArray("textureID", &levelTextureIDs[offset], renderableSize);
		levelShader->setInt2Array("gPos", &levelDrawables[2 * offset], renderableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderableSize);
		offset += renderableSize;
	}

	offset = 0;
	enemyShader->use();
	glBindVertexArray(enemyVAO);
	for (auto renderableSize : enemyRenderableCount) {
		enemyShader->setIntArray("textureID", &enemyTextureIDs[offset], renderableSize);
		enemyShader->setIntArray("carryGold", &enemyGoldIndicator[offset], renderableSize);
		enemyShader->setVec2Array("gPos", &enemyDrawables[2 * offset], renderableSize);
		enemyShader->setIntArray("direction", &enemyDirections[offset], renderableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderableSize);
		offset += renderableSize;
	}

	offset = 0;
	characterShader->use();
	glBindVertexArray(characterVAO);	
	for (auto renderableSize : characterRenderableCount) {
		characterShader->setIntArray("textureID", &characterTextureIDs[offset], renderableSize);
		characterShader->setVec2Array("gPos", &characterDrawables[2 * offset], renderableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderableSize);
		offset += renderableSize;
	}

	renderTouchButtons();
}

void EmscriptenRenderingManager::renderGenerator() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int offset = 0;
	levelShader->use();
	glBindVertexArray(levelVAO);
	for (auto renderableSize : generatorRenderableCount) {
		levelShader->setIntArray("textureID", &generatorTextures[offset], renderableSize);
		levelShader->setInt2Array("gPos", &generatorDrawables[2 * offset], renderableSize);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderableSize);
		offset += renderableSize;
	}
	
	renderTouchButtons();
}

void EmscriptenRenderingManager::renderMainMenu(int menuCursor, int gameVersion) {
	RenderingManager::renderMainMenu(menuCursor, gameVersion);

	renderTouchButtons();
}

void EmscriptenRenderingManager::renderTouchButtons() {
	if (EmscriptenHandler::is_mobile()) {
		emscriptenIOContext->activateDisplayViewPort();
		touchInputShader->use();
		glBindVertexArray(touchVAO);
		glDrawElements(GL_TRIANGLES, 6 * TOUCH_BUTTON_NR, GL_UNSIGNED_INT, 0);
		emscriptenIOContext->activateCanvasViewPort();
	}
}

void EmscriptenRenderingManager::setUpRenderableCount(std::vector<int>& renderableCount, int drawableSize, int divider) {
	int maxUniform = (maxVertexUniformNumber < maxFragmentUniformNumber ? maxVertexUniformNumber : maxFragmentUniformNumber) / divider;
	int fullRenderCount = drawableSize / maxUniform;
	int remainingRenderable = drawableSize % maxUniform;

	renderableCount = {};

	for (int i = 0; i < fullRenderCount; i++)
	{
		renderableCount.push_back(maxUniform);
	}

	if (remainingRenderable > 0) {
		renderableCount.push_back(remainingRenderable);
	}
}

void EmscriptenRenderingManager::initializeLevelLayout() {
	RenderingManager::initializeLevelLayout();
	setUpRenderableCount(levelRenderableCount, currentLevelDrawableSize, 1);
}

void EmscriptenRenderingManager::initializeEnemies() {
	RenderingManager::initializeEnemies();
	setUpRenderableCount(enemyRenderableCount, enemyDrawableSize, 3);
}

void EmscriptenRenderingManager::initializeCharacters() {
	RenderingManager::initializeCharacters();
	setUpRenderableCount(characterRenderableCount, characterDrawableSize, 1);
}

void EmscriptenRenderingManager::enableFinishingLadderDrawing() {
	RenderingManager::enableFinishingLadderDrawing();
	setUpRenderableCount(levelRenderableCount, currentLevelDrawableSize, 1);
}

void EmscriptenRenderingManager::clearRenderableObjects() {
	RenderingManager::clearRenderableObjects();

	setUpRenderableCount(levelRenderableCount, 0, 1);
	setUpRenderableCount(enemyRenderableCount, 0, 1);
	setUpRenderableCount(characterRenderableCount, 0, 1);
}

void EmscriptenRenderingManager::setupTouchButtonRenderer() {
	loadTexture(6, touchTexture, assetFolder + "Texture/Touch.png");

	touchInputShader->use();
	touchInputShader->setInt("touchTexture", 6);

	for (int i = 0; i < TOUCH_BUTTON_NR; i++) {
		touchIndices[6 * i + 0] = 4 * i + 0;
		touchIndices[6 * i + 1] = 4 * i + 2;
		touchIndices[6 * i + 2] = 4 * i + 1;

		touchIndices[6 * i + 3] = 4 * i + 2;
		touchIndices[6 * i + 4] = 4 * i + 3;
		touchIndices[6 * i + 5] = 4 * i + 1;
	}

	glGenVertexArrays(1, &touchVBO);
	glGenBuffers(1, &touchVBO);
	glGenBuffers(1, &touchEBO);

	glBindVertexArray(touchVBO);

	glBindBuffer(GL_ARRAY_BUFFER, touchVBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, touchEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * TOUCH_BUTTON_NR, touchIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void EmscriptenRenderingManager::updateTouchButtonRenderer(float vertices[TOUCH_BUTTON_NR * 16]) {
	glBindVertexArray(touchVAO);
	glBindBuffer(GL_ARRAY_BUFFER, touchVBO);
	glBufferData(GL_ARRAY_BUFFER, TOUCH_BUTTON_NR * 16 * sizeof(float), vertices, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void EmscriptenRenderingManager::terminate() {
	RenderingManager::terminate();

	glDeleteVertexArrays(1, &touchVAO);
	glDeleteBuffers(1, &touchVBO);
	glDeleteBuffers(1, &touchEBO);

	glDeleteTextures(1, &touchTexture);

	delete touchInputShader;
}