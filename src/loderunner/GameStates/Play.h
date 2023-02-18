#ifndef PLAY_H
#define PLAY_H

#include "GameState.h"

#include <memory>
#include "Brick.h"
#include "Trapdoor.h"

#include "Rendering/RenderingManager.h"

class Shader;
class Text;

class Play : public GameState {
private:
	//std::unique_ptr<Brick> brick[30][18];
	std::shared_ptr<Brick>** brick = nullptr;

	std::shared_ptr<Trapdoor>** trapdoors = nullptr;
	LayoutBlock** layout = nullptr;	
	
	std::vector<Vector2DInt> finishingLadders;
	short highestLadder = 30;
	
	void handleNonControlButtons();

	std::shared_ptr<RenderingManager> renderingManager;

	std::shared_ptr<std::vector<std::shared_ptr<Brick>>> brickList;

	std::shared_ptr<Text> timeText;

public:
	Play();
	void drawLevel();
	void start() override;
	void update(float) override;
	void end() override;
	void generateFinishingLadders();
	short getHighestLadder();

	void loadLevel(unsigned int);
	void transitionToDeath();
	void transitionToOutro(short, short, short);
	void clearContainers();
	void setLadders(int, std::vector<Vector2DInt>);

	void setRenderingManager(std::shared_ptr<RenderingManager> renderingManager)
	{
		this->renderingManager = renderingManager;
	}
};

#endif