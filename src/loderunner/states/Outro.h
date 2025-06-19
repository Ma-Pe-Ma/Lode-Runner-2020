#ifndef OUTRO_H
#define OUTRO_H

#include "State.h"
#include <string>
#include <memory>
#include <optional>

class RenderingManager;
class Player;
class Text;

class Outro : public State {
private:
	std::shared_ptr<Text> goldPoints = nullptr;
	std::shared_ptr<Text> enemyPoints = nullptr;
	std::shared_ptr<Text> totalPoints = nullptr;

	short enemyScore = 0;
	short goldScore = 0;
	std::optional<int> fruitID;

	void setupRenderingManager();

	std::shared_ptr<Player> player;

	std::chrono::system_clock::time_point previousFrame;
public:
	void start() override;
	void update() override;
	void end() override;

	void setScoreParameters(short, short, std::optional<int>);
};

#endif