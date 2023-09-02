#ifndef ENEMYSTATE_H
#define ENEMYSTATE_H

enum class EnemyState {
	freeRun,
	startingToFall,
	falling,
	digging,
	pitting,
	dying
};

#endif // !ENEMYSTATE_H
