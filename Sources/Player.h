#ifndef PLAYER_H
#define PLAYER_H

#include "Enemy.h"

class Player final : public Enemy {
private:
	void FindPath() override;

	//variable for changing moving sound, going-0, laddering-1, poling-2
	int going[3] = {};
	int idleTimeStart;

	//Moving
	void FreeRun() override;
	void Digging() override;
	void InitiateFallingStart() override;
	void InitiateFallingStop() override;

	void Falling() override;
	virtual bool CheckHole() override;

	//Animating
	inline void AnimateFreeRun() override;
	inline void AnimateDigging() override;
	inline void AnimateDying() override;
	inline void AnimateFalling() override;
	inline void AnimateGoing() override;
	inline void AnimateOnLadder() override;
	inline void AnimateOnPole() override;
	
	inline void CheckGoldCollect() override;
	inline void CheckGoldDrop() override {};
	void ReleaseFromDigging() override;

	virtual void CheckCollisionWithOthers() override {}
public:
	static void AddPlayer(Vector2DInt);
	Player();
	void Die() override;
	void Dying() override;
};

#endif