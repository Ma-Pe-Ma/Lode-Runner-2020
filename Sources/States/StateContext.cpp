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
    transitionTo(mainMenu);

    level[0] = startingLevel;
    level[1] = startingLevel;
}

void StateContext::transitionTo(State* newState, bool start, bool end) {
    if (end) {
        currentState->end();
    }

    currentState = newState;
    currentState->setStateContext(this);

    if (start) {
        currentState->start();
    }
}

void StateContext::update(float currentFrame) {
    currentState->update(currentFrame);
}

void StateContext::transitionToAtEndOfFrame(State* newState, bool start, bool end) {

}