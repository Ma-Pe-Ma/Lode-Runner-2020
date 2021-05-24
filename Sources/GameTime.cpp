#include "GameTime.h"

float GameTime::curSessionStartTime = 0;
float GameTime::prevSessionSum = 0;
float GameTime::gameTime = 0;

int GameTime::FPS = 35;
float GameTime::currentFrame = 0;
float GameTime::deltaTime = 0.0f;	// time between current frame and last frame
float GameTime::lastFrame = 0.0f;
float GameTime::speed = 0;
float GameTime::speedFactor = 5;

void GameTime::reset() {
	GameTime::prevSessionSum = 0;
	GameTime::gameTime = 0;
}

void GameTime::setSessionStartTime() {
	GameTime::curSessionStartTime = GameTime::currentFrame;
}

void GameTime::setSessionEndTime() {
	GameTime::prevSessionSum = GameTime::gameTime;
}

void GameTime::update(float currentFrame) {
	GameTime::gameTime = GameTime::prevSessionSum + (currentFrame - GameTime::curSessionStartTime);
}

float GameTime::getGameTime() {
	return GameTime::gameTime;
}

float GameTime::getSpeed() {
	return GameTime::speed;
}

int GameTime::getFPS() {
	return GameTime::FPS;
}

void GameTime::setFPS(unsigned int FPS) {
	GameTime::FPS = FPS;
}

void GameTime::calculateTimeValues(float currentFrame) {
	GameTime::currentFrame = currentFrame;

	GameTime::deltaTime = currentFrame - GameTime::lastFrame;
	GameTime::lastFrame = currentFrame;

	GameTime::speed = GameTime::speedFactor * GameTime::deltaTime;
}

float GameTime::getCurrentFrame() {
	return GameTime::currentFrame;
}
float GameTime::getDeltaTime() {
	return GameTime::deltaTime;
}