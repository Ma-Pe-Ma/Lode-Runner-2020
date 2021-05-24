#include "States/StateContext.h"

StateContext::StateContext() {
    this->mainMenu = new MainMenu();
    this->intro = new Intro();
    this->gamePlay = new GamePlay();
    this->select = new Select();
    this->outro = new Outro();
    this->gameOver = new GameOver();
    this->generator = new Generator();
    currentState = mainMenu;
    TransitionTo(mainMenu);

    level[0] = startingLevel;
    level[1] = startingLevel;
}

void StateContext::TransitionTo(State* newState, bool start, bool end) {
    if (end) {
        currentState->end();
    }

    currentState = newState;
    currentState->SetStateContext(this);

    if (start) {
        currentState->start();
    }
}

void StateContext::update(float currentFrame) {
    currentState->update(currentFrame);
}

void StateContext::TransitionToAtEndOfFrame(State* newState, bool start, bool end) {

}