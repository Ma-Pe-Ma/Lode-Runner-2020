#include "Enemy.h"
#include "Gold.h"
#include "GameTime.h"

std::vector<std::unique_ptr<Gold>> Gold::uncollectedGold;
std::vector<std::unique_ptr<Gold>> Gold::collectedGold;

short Gold::getCollectedSize() {
	return collectedGold.size();
}
short Gold::getUncollectedSize() {
	return uncollectedGold.size();
}

void Gold::addGoldToCollected(std::unique_ptr<Gold> collectedGold) {
	Gold::collectedGold.push_back(std::move(collectedGold));
}

void Gold::clearGoldHolders() {
	uncollectedGold.clear();
	collectedGold.clear();
}

void Gold::initialize(int indexIn, Vector2D pos) {
	this->pos = pos;
}

void Gold::drawGolds() {
	for (auto& gol : uncollectedGold) {
		gol->draw();
	}
}

void Gold::addGoldToUncollected(std::unique_ptr<Gold> newGold) {
	uncollectedGold.push_back(std::move(newGold));
}

bool Gold::goldChecker(int x, int y) {
	for (auto& gol : uncollectedGold) {
		if (gol->pos.x == x && gol->pos.y == y) {
			return true;
		}
	}

	return false;
}

std::unique_ptr<Gold> Gold::goldCollectChecker(float x, float y) {
	for (auto it = uncollectedGold.begin(); it != uncollectedGold.end(); it++) {
		if (abs(it->get()->pos.x - x) < 0.15f && abs(it->get()->pos.y - y) < 0.15f) {
			std::unique_ptr<Gold> foundGold = std::move(*it);
			uncollectedGold.erase(it);

			//return std::move(foundGold);
			return foundGold;
		}
	}

	return nullptr;
}

void Gold::draw() {
	int timeFactor = int(GameTime::getGameTime()) % 4;

	if (timeFactor == 3) {
		timeFactor = 1;
	}

	Drawing::drawLevel(pos.x, pos.y, 36 + timeFactor);
}

bool Gold::shouldBeReleased() {
	//std::cout << "\n relasecounter: " << releaseCounter;

	if (releaseCounter-- <= 0) {
		return true;
	}

	return false;
}