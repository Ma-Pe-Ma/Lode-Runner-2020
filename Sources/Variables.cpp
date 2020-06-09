#include "Variables.h"

Shader *selectShader, *mainShader, *levelShader, *playerShader;

Button up, down, leftButton, rightButton, space, enter, pButton, leftDigButton, rightDigButton, REC, lAlt;

unsigned int pauseScreenT;
unsigned int selectVBO, selectVAO, selectEBO;
unsigned int levelVBO, levelVAO, levelEBO;
unsigned int playerVBO, playerVAO, playerEBO;
unsigned int mainVBO, mainVAO, mainEBO;
unsigned int cursorVAO, cursorVBO, cursorEBO;

std::string levelFileName = "level/lodeRunner.baseLevel.txt";

char pcmout[SOUND_FILE_NR][FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];
unsigned int SCR_WIDTH = 1500;
unsigned int SCR_HEIGHT = 900;

unsigned int former_SCR_WIDTH = 1500;
unsigned int former_SCR_HEIGHT = 900;

unsigned int windowPosX = 30;
unsigned int windowPosY = 30;

unsigned int viewPortWidth = SCR_WIDTH;
unsigned int viewPortHeight = SCR_HEIGHT;

unsigned int viewPortX = 0;
unsigned int viewPortY = 0;

const char soundNames[SOUND_FILE_NR][25] = { "SFX/gold.ogg", "SFX/dig.ogg","SFX/dig_prev.ogg",
										"SFX/death.ogg", "SFX/everygold.ogg","SFX/mainmenu.ogg",
										"SFX/gameover.ogg", "SFX/gameplay.ogg","SFX/intro.ogg",
										"SFX/step1.ogg","SFX/step2.ogg","SFX/ladder1.ogg",
										"SFX/ladder2.ogg", "SFX/outro.ogg","SFX/pause.ogg",
										"SFX/pole1.ogg","SFX/pole2.ogg", "SFX/fall.ogg"
};

RecordingState recordingState;

//variable for changing moving sound, going-0, laddering-1, poling-2
 int going[3] = {};

 bool championShip = false;
 bool usCover = false;

 int FPS = 35;
 unsigned int recordinghHeight = 0;

#ifdef VIDEO_RECORDING
 MultiMedia* GameVideo;
 VideoParameters* videoIn;
 VideoParameters* videoOut;
 AudioParameters* audioIn;
 AudioParameters* audioOut;
#endif

 bool startingScreen;
 float startingTimer;

 float deltaTime = 0.0f;	// time between current frame and last frame
 float lastFrame = 0.0f;
 float speed;
 float speedFactor = 5;

 float diggingTime = 7.15f;
 float holeIdle = 2.2f;

 float playerSpeed = 0.9f;
 float enemySpeed = 0.415f;

 float destroyTime = 0.5f;
 float buildTime = 0.5f;
 float holeHorizontalTime = 0.25;
 int randomDebris;

//current curosr in the generating screen
 int geX, geY;

//the highest Ladder of the level, where the player has to reach to exit, it is determined at every level loading
 int highestLadder;

//cursor of the menu
 int menuCursor = 0;

//number of earlier taken screenshots and videos
 unsigned int scr = 0;
 unsigned int vid = 0;

 LayoutBlock layout[30][18];
 int gen[30][18] = {};

//array which holds the timer of holes
 float holeTimer[30][18];

 float introTimer;
 float gameTime;
 float curSessionStartTime = 0;
 float outroTimer;
 float game_overTimer;
 float prevSessionSum = 0;
 float recordStartTime = 0;

 int correction = 0;

 int score_gold = 0;
 int score_enemy = 0;
 int level[2] = { 1,1 };
 int playerLife[2] = { 5,5 };
 int playerNr = 0;
 int score[2] = { 0,0 };
 int highScore = 0;
 bool game_over[2] = {};
bool level_inc[2][2];

bool right_pit;
bool left_pit;
float idleTime;
float fallTime;
float digTime;

bool fullScreen = false;
GLFWwindow* window;

float blokk[16] =
{
	-0.5,	-0.5,
	 0.5,	-0.5,
	 0.5, 	 0.5,
	-0.5,	 0.5,
};

float main_menu[16] =
{ -1,	-1,		0.0, 0.0,
	 1, -1,		1.0, 0.0,
	 1,	 1,		1.0, 1.0,
	-1,	 1,		0.0, 1.0,
};

unsigned int indices[6] = {
		 0,1,2,
		 2,3,0
};

Menu menu = L01;