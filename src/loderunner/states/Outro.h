#ifndef OUTRO_H
#define OUTRO_H

#include <optional>
#include <string>
#include <memory>

#include "State.h"

#include "gameplay/GameElements.h"

class RenderingManager;
class Enemy;
class Text;

class Outro : public State {
private:
	std::shared_ptr<Text> goldPoints = nullptr;
	std::shared_ptr<Text> enemyPoints = nullptr;
	std::shared_ptr<Text> totalPoints = nullptr;

	short enemyScore = 0;
	short goldScore = 0;
	short bonusScore = 0;
	std::optional<Fruit> fruit;

	void setupRenderingManager();

	std::shared_ptr<Enemy> player;

	std::chrono::system_clock::time_point previousFrame;
public:
	void start() override;
	void update() override;
	void end() override;

	void setScoreParameters(short, short, std::optional<Fruit>);
};

#endif