#include "States/StateContext.h"


StateContext::StateContext() {
    this->mainMenu = new MainMenu();
    this->mainMenu->setStateContext(this);

    this->intro = new Intro();
    this->intro->setStateContext(this);

    this->gamePlay = new GamePlay();
    this->gamePlay->setStateContext(this);

    this->select = new Select();
    this->select->setStateContext(this);

    this->outro = new Outro();
    this->outro->setStateContext(this);

    this->gameOver = new GameOver();
    this->gameOver->setStateContext(this);

    this->generator = new Generator();
    this->generator->setStateContext(this);

    currentState = mainMenu;
    transitionTo(mainMenu);

    level[0] = IOHandler::startingLevel;
    level[1] = IOHandler::startingLevel;
}

void StateContext::setRenderingManager(std::shared_ptr<RenderingManager> renderingManager)
{
    this->mainMenu->setRenderingManager(renderingManager);
    this->intro->setRenderingManager(renderingManager);
    this->gamePlay->setRenderingManager(renderingManager);
    this->select->setRenderingManager(renderingManager);
    this->outro->setRenderingManager(renderingManager);
    this->gameOver->setRenderingManager(renderingManager);
    this->generator->setRenderingManager(renderingManager);
}

void StateContext::transitionTo(State* newState, bool start, bool end) {
    if (end) {
        currentState->end();
    }

    currentState = newState;
    //currentState->setStateContext(this);

    if (start) {
        currentState->start();
    }
}

void StateContext::update(float currentFrame) {
    currentState->update(currentFrame);
    checkTransitionAtEndofFrame();
}

void StateContext::transitionToAtEndOfFrame(State* newState, bool start, bool end) {

    transitionableAtEndOfFrame = newState;
}

void StateContext::checkTransitionAtEndofFrame()
{
    if (transitionableAtEndOfFrame != nullptr)
    {
        transitionTo(transitionableAtEndOfFrame);
        transitionableAtEndOfFrame = nullptr;
    }
}