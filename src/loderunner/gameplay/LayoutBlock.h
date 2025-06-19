#ifndef LAYOUTBLOCK_H
#define LAYOUTBLOCK_H

enum class LayoutBlock {
	empty,
	brick,
	concrete,
	ladder,
	pole,
	trapDoor,
	finishLadder,
};

enum class EnemyState {
	freeRun,
	falling,
	digging,
	pitting,
	dying
};

#endif // !LAYOUTBLOCK_H
