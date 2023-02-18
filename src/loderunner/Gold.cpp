#include "Enemy.h"
#include "Gold.h"
#include "GameTime.h"

std::vector<std::shared_ptr<Gold>> Gold::uncollectedGold;
std::vector<std::shared_ptr<Gold>> Gold::collectedGold;

short Gold::getCollectedSize() {
	return collectedGold.size();
}
short Gold::getUncollectedSize() {
	return uncollectedGold.size();
}

void Gold::addGoldToCollected(std::shared_ptr<Gold> collectedGold) {
	collectedGold->setPos(Vector2DInt{ -1, -1 });
	Gold::collectedGold.push_back(collectedGold);
}

void Gold::clearGoldHolders() {
	uncollectedGold.clear();
	collectedGold.clear();
}

void Gold::addGoldToUncollected(std::shared_ptr<Gold> newGold) {
	uncollectedGold.push_back(newGold);
}

bool Gold::goldChecker(int x, int y) {
	for (auto& gol : uncollectedGold) {
		if (gol->pos.x == x && gol->pos.y == y) {
			return true;
		}
	}

	return false;
}

std::shared_ptr<Gold> Gold::goldCollectChecker(float x, float y) {
	for (auto it = uncollectedGold.begin(); it != uncollectedGold.end(); it++) {
		if (std::abs(it->get()->pos.x - x) < 0.15f && std::abs(it->get()->pos.y - y) < 0.15f) {
			std::shared_ptr<Gold> foundGold = *it;
			uncollectedGold.erase(it);

			return foundGold;
		}
	}

	return nullptr;
}

bool Gold::shouldBeReleased() {
	if (releaseCounter-- <= 0) {
		return true;
	}

	return false;
}