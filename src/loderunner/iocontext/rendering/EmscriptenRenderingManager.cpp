#include "EmscriptenRenderingManager.h"

#include "../EmscriptenIOContext.h"

void EmscriptenRenderingManager::createShaders() {
	RenderingManager::createShaders();

	if (EmscriptenHandler::is_mobile()) {
		touchInputShader = new Shader(assetFolder + "/shaders/touch.vs", assetFolder + "/shaders/touch.fs");
		setupTouchButtonRenderer();
	}

	int width;
	int height;
	emscripten_get_canvas_element_size("canvas", &width, &height);
	emscriptenIOContext->framebufferSizeCallback(width, height);
}

void EmscriptenRenderingManager::render() {
	RenderingManager::render();
	renderTouchButtons();
}

void EmscriptenRenderingManager::renderGenerator() {
	RenderingManager::renderGenerator();	
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

void EmscriptenRenderingManager::setupTouchButtonRenderer() {
	loadTexture(6, touchTexture, assetFolder + "textures/Touch.png");

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