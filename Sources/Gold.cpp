#include "Enemy.h"
#include "Gold.h"
#include "GameTime.h"

std::vector<std::unique_ptr<Gold>> Gold::uncollectedGold;
std::vector<std::unique_ptr<Gold>> Gold::collectedGold;

short Gold::GetCollectedSize() {
	return collectedGold.size();
}
short Gold::GetUncollectedSize() {
	return uncollectedGold.size();
}

void Gold::addGoldToCollected(std::unique_ptr<Gold> collectedGold) {
	Gold::collectedGold.push_back(std::move(collectedGold));
}

void Gold::ClearGoldHolders() {
	uncollectedGold.clear();
	collectedGold.clear();
}

void Gold::Initialize(int indexIn, Vector2D PosIn) {
	Pos = PosIn;
}

void Gold::DrawGolds() {
	for (auto& gol : uncollectedGold) {
		gol->Draw();
	}
}

void Gold::addGoldToUncollected(std::unique_ptr<Gold> newGold) {
	uncollectedGold.push_back(std::move(newGold));
}

bool Gold::GoldChecker(int x, int y) {
	for (auto& gol : uncollectedGold) {
		if (gol->Pos.x == x && gol->Pos.y == y) {
			return true;
		}
	}

	return false;
}

std::unique_ptr<Gold> Gold::GoldCollectChecker(float x, float y) {
	for (auto it = uncollectedGold.begin(); it != uncollectedGold.end(); it++) {
		if (abs(it->get()->Pos.x - x) < 0.15f && abs(it->get()->Pos.y - y) < 0.15f) {
			std::unique_ptr<Gold> foundGold = std::move(*it);
			uncollectedGold.erase(it);

			//return std::move(foundGold);
			return foundGold;
		}
	}

	return nullptr;
}

void Gold::Draw() {
	int timeFactor = int(GameTime::getGameTime()) % 4;

	if (timeFactor == 3) {
		timeFactor = 1;
	}

	DrawLevel(Pos.x, Pos.y, 36 + timeFactor);
}

bool Gold::shouldBeReleased() {
	//std::cout << "\n relasecounter: " << releaseCounter;

	if (releaseCounter-- <= 0) {
		return true;
	}

	return false;
}