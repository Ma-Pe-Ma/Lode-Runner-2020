#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>

#include "Button.h"
#include "Enums/LayoutBlock.h"
#include "Enums/Menu.h"

extern Button up, down, leftButton, rightButton, space, enter, pButton, leftDigButton, rightDigButton, REC, lAlt;

extern unsigned int pauseScreenT;

extern std::string levelFileName;

//variable for changing moving sound, going-0, laddering-1, poling-2
extern int going[3];

extern bool championShip;
extern bool usCover;

extern int FPS;
extern unsigned int recordingHeight;

extern bool startingScreen;
extern float startingTimer;

extern float deltaTime;	// time between current frame and last frame
extern float lastFrame;
extern float speed;
extern float speedFactor;

extern float diggingTime;
extern float holeIdle;

extern float playerSpeed;
extern float enemySpeed;

extern float destroyTime;
extern float buildTime;
extern float holeHorizontalTime;
extern int randomDebris;

//current curosr in the generating screen
extern int geX, geY;

//the highest Ladder of the level, where the player has to reach to exit, it is determined at every level loading
extern int highestLadder;

//cursor of the menu
extern int menuCursor;

extern LayoutBlock layout[30][18];
extern int gen[30][18];

//array which holds the timer of holes
extern float holeTimer[30][18];

extern float introTimer;
extern float gameTime;
extern float curSessionStartTime;
extern float outroTimer;
extern float game_overTimer;
extern float prevSessionSum;
extern float recordStartTime;

extern int correction;

extern int score_gold;
extern int score_enemy;
extern int level[2];
extern int playerLife[2];
extern int playerNr;
extern int score[2];
extern int highScore;
extern bool game_over[2];
extern bool level_inc[2][2];

extern bool right_pit;
extern bool left_pit;
extern float idleTime;
extern float fallTime;
extern float digTime;

extern float blokk[16];

extern float main_menu[16];

extern const unsigned int indices[6];

extern Menu menu;

#endif