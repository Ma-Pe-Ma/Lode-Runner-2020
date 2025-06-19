#include "GameElements.h"

#include "Brick.h"
#include "Gold.h"
#include "Enemy.h"
#include "LayoutBlock.h"

bool GameElements::isBrick(int x, int y) {
    return std::find_if(brickList.begin(), brickList.end(), [x, y](std::shared_ptr<Brick> brick) {
        return brick->position.x == x && brick->position.y == y;
        }) != brickList.end();
    }

bool GameElements::isEnemy (float x, float y, float thresholdX, float thresholdY) {
    return std::find_if(enemies.begin(), enemies.end(), [x, y, thresholdX, thresholdY](std::shared_ptr<Enemy> enemy) {
        return std::abs(enemy->pos.x - x) <= thresholdX && std::abs(enemy->pos.y - y) <= thresholdY;
        }) != enemies.end();		
};

bool GameElements::isGold (int x, int y) {
    return std::find_if(uncollectedGoldList.begin(), uncollectedGoldList.end(), [x, y](std::shared_ptr<Gold> gold) {
        return gold->pos.x == x && gold->pos.y == y;
        }) != uncollectedGoldList.end();
};

Vector2DInt GameElements::getPlayerPosition() {
    return this->player->current;
}

EnemyState GameElements::getPlayerState() {
    return this->player->state;
}

bool GameElements::isEnemyUnder(Enemy* checkableEnemy) {
    for (auto& enemy : enemies) {
        if (enemy.get() == checkableEnemy) {
            continue;
        }

        if (enemy->current.y == checkableEnemy->pos.y - 1 && std::abs(enemy->pos.x - checkableEnemy->pos.x) < 0.55f) {
            return true;
        }
    }

    return false;
}

bool GameElements::isEnemyUnderNext(Enemy* checkableEnemy, float nextPos) {
    for (auto& enemy : enemies) {
        if (enemy.get() == checkableEnemy) {
            continue;
        }

        if (enemy->current.y == checkableEnemy->pos.y - 1 && std::abs(enemy->current.x - nextPos) <= 1.0f) {
            return true;
        }
    }
    
    return false;
}

bool GameElements::isEnemyUnderFalling(Enemy* checkableEnemy, float nextPos) {
    for (auto& enemy : enemies) {
        if (enemy.get() == checkableEnemy) {
            continue;
        }

        if (checkableEnemy->pos.x == enemy->current.x && enemy->pos.y < checkableEnemy->pos.y && nextPos - int(enemy->pos.y + 0.5f) < 1.0f) {
            return true;
        }
    }

    return false;
}

bool GameElements::isEnemyNearHole(Enemy* checkableEnemy, float x, float y) {
    for (auto& enemy : enemies) {
        if (enemy.get() == checkableEnemy) {
            continue;
        }

        if (std::abs(enemy->pos.x - x) < 0.5f && enemy->pos.y < y + 2) {
            return true;
        }
    }

    return false;
}