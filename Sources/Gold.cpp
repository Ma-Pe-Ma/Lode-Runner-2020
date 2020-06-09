#include "Enemy.h"
#include "Gold.h"

int Gold::goldNr = 0;
int Gold::remainingGoldCount = 0;

Gold Gold::gold[100];

void Gold::Initialize(int indexIn, Vector2D PosIn) {
	Pos = PosIn;
	carrier = nullptr;
	collected = false;
	index = indexIn;
}