#ifndef STATECONTEXT_H
#define STATECONTEXT_H

#include "State.h"

#include "MainMenu.h"
#include "Intro.h"
#include "GamePlay.h"
#include "Select.h"
#include "Outro.h"
#include "GameOver.h"
#include "Generator.h"

#include "IOHandler.h"

class StateContext {
private:
	State* currentState;

public:
	MainMenu* mainMenu;
	Intro* intro;
	GamePlay* gamePlay;
	Select* select;
	Outro* outro;
	GameOver* gameOver;
	Generator* generator;

	//void TransitionTo(State*);
	void TransitionTo(State*, bool start = true, bool end = true);
	void TransitionToAtEndOfFrame(State*, bool start = true, bool end = true);
	StateContext();
	
	void update(float);

	//cursor of the menu
	int menuCursor = 0;
	int level[2] = { 1,1 };
	int playerLife[2] = { 5,5 };
	int score[2] = { 0,0 };

	int playerNr = 0;
	int highScore = 0;
};

#endif