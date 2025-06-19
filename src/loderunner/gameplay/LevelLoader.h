#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <map>
#include <memory>

class IOContext;
class GameContext;
class RenderingManager;
class GameElements;

struct Fruit;

class LevelLoader {
	std::shared_ptr<IOContext> ioContext;
	GameContext* gameContext;
	std::shared_ptr<RenderingManager> renderingManager;

	std::shared_ptr<GameElements> gameElements;
public:
	LevelLoader(std::shared_ptr<IOContext>, std::shared_ptr<RenderingManager>, GameContext*);

	std::shared_ptr<GameElements> getGameElements() { return this->gameElements; }

	void loadLevel(std::array<std::array<char, 28>, 16>, bool generating = false);
	void generateLevel(std::array<std::array<short, 28>, 16> gen);
};

#endif