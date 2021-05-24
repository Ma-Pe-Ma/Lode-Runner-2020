#include "GameStates/Begin.h"
#include "States/GamePlay.h"

#include "GameTime.h"
#include "Enemy.h"

void Begin::start() {
	startTime = GameTime::getCurrentFrame();
	Audio::SFX[7].PlayPause();
	GameTime::reset();
}

void Begin::update(float currentFrame) {
	Enemy::DrawPaused();
	Gold::DrawGolds();

	gamePlay->play->drawLevel();
	gamePlay->WriteGameTime();

	if (currentFrame - startTime > 2.0f) {
		gamePlay->TransitionTo(gamePlay->play);
	}	
}

void Begin::end() {

}