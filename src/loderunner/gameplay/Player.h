#ifndef PLAYER_H
#define PLAYER_H

#include "Enemy.h"
class IOContext;

class Player final : public Enemy {
private:
	void findPath() override;

	//variable for changing moving sound, going-0, laddering-1, poling-2
	std::array<short, 3> going = {};
	int idleTimeStart;

	//Moving
	void freeRun() override;
	void digging() override;
	void initiateFallingStart() override;
	void initiateFallingStop() override;

	void falling() override;
	virtual bool checkHole() override;

	//Animating
	inline void animateFreeRun() override;
	inline void animateDigging() override;
	//inline void animateDying() override;
	//inline void animateFalling() override;
	inline void animateGoing() override;
	inline void animateOnLadder() override;
	inline void animateOnPole() override;
	
	inline void checkGoldCollect() override;
	inline void checkGoldDrop() override {};

	virtual void checkCollisionWithOthers() override {}

	std::shared_ptr<IOContext> ioContext;
public:
	void releaseFromDigging() override;

	Player(float, float);
	void die() override;
	void dying() override;
	void setCharSpeed(float) override;
	void setPosition(Vector2D pos)
	{
		this->pos = pos;

		positionPointer[0] = pos.x;
		positionPointer[1] = pos.y;
	}

	void setIOContext(std::shared_ptr<IOContext> ioContext) { this->ioContext = ioContext; }
};

#endif