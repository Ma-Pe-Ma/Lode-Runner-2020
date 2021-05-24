#ifndef GAMETIME_H
#define GAMETIME_H

class GameTime {
	static float curSessionStartTime;
	static float prevSessionSum;
	static float gameTime;
	static int FPS;
	static float currentFrame;
	static float deltaTime;	// time between current frame and last frame
	static float lastFrame;
	static float speed;
	static float speedFactor;

public:
	static void reset();
	static void setSessionStartTime();
	static void update(float);
	static float getGameTime();
	static void setSessionEndTime();
	static float getSpeed();
	static int getFPS();
	static void setFPS(unsigned int);
	static float getCurrentFrame();
	static void calculateTimeValues(float);
	static float getDeltaTime();

};
#endif // !GAMETIME_H
