#ifndef EMSCRIPTENRENDERINGMANAGER_H
#define EMSCRIPTENRENDERINGMANAGER_H

#include "RenderingManager.h"

#include "../EmscriptenIOContext.h"

class EmscriptenIOContext;

class EmscriptenRenderingManager : public RenderingManager {
	Shader* touchInputShader;
	unsigned int touchVAO, touchVBO, touchEBO;
	unsigned int touchTexture;
	unsigned int touchIndices[6 * TOUCH_BUTTON_NR];


	void setupTouchButtonRenderer();

	void renderTouchButtons();

	std::shared_ptr<EmscriptenIOContext> emscriptenIOContext;
public:
	EmscriptenRenderingManager(std::string assetFolder, std::shared_ptr<IOContext> ioContext) : RenderingManager(assetFolder, ioContext) {
		this->emscriptenIOContext = std::static_pointer_cast<EmscriptenIOContext>(ioContext);
	}

	void createShaders() override;
	
	void render() override;
	void renderGenerator() override;
	void renderMainMenu(int menuCursor, int gameVersion) override;

	void terminate() override;

	void updateTouchButtonRenderer(float[TOUCH_BUTTON_NR *16]);
};

#endif