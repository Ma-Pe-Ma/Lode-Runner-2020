#ifndef VARIABLES_H
#define VARIABLES_H

#define SOUND_FILE_NR 18
#define FRAMES_PER_BUFFER 64
#define CHANNEL_COUNT 2
#define SAMPLE_RATE 44100

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <SHADER/shader_m.h>

#include "Button.h"
#include "Enums/LayoutBlock.h"
#include "Enums/RecordingState.h"
#include "Enums/Menu.h"

extern Shader *selectShader, *mainShader, *levelShader, *playerShader;

extern Button up, down, leftButton, rightButton, space, enter, pButton, leftDigButton, rightDigButton, REC, lAlt;

extern unsigned int pauseScreenT;
extern unsigned int selectVBO, selectVAO, selectEBO;
extern unsigned int levelVBO, levelVAO, levelEBO;
extern unsigned int playerVBO, playerVAO, playerEBO;
extern unsigned int mainVBO, mainVAO, mainEBO;
extern unsigned int cursorVAO, cursorVBO, cursorEBO;

extern std::string levelFileName;

extern char pcmout[SOUND_FILE_NR][FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

extern unsigned int former_SCR_WIDTH;
extern unsigned int former_SCR_HEIGHT;

extern unsigned int windowPosX;
extern unsigned int windowPosY;

extern unsigned int viewPortWidth;
extern unsigned int viewPortHeight;

extern unsigned int viewPortX;
extern unsigned int viewPortY;

extern const char soundNames[SOUND_FILE_NR][25];

extern RecordingState recordingState;

//variable for changing moving sound, going-0, laddering-1, poling-2
extern int going[3];

extern bool championShip;
extern bool usCover;

extern int FPS;
extern unsigned int recordinghHeight;

#ifdef VIDEO_RECORDING
extern MultiMedia* GameVideo;
extern VideoParameters* videoIn;
extern VideoParameters* videoOut;
extern AudioParameters* audioIn;
extern AudioParameters* audioOut;
#endif

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

//number of earlier taken screenshots and videos
extern unsigned int scr;
extern unsigned int vid;

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

extern bool fullScreen;
extern GLFWwindow* window;

extern float blokk[16];

extern float main_menu[16];

extern unsigned int indices[6];

extern Menu menu;

#endif