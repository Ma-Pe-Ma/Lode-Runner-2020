#include "states/StateContext.h"
#include "iocontext/IOContext.h"

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
}

void StateContext::initialize()
{
    currentState = mainMenu;
    transitionTo(mainMenu);
}

void StateContext::setRenderingManager(std::shared_ptr<RenderingManager> renderingManager)
{
    this->renderingManager = renderingManager;    
}

void StateContext::setIOContext(std::shared_ptr<IOContext> ioContext)
{
    this->ioContext = ioContext;
}

void StateContext::setAudio(std::shared_ptr<Audio> audio)
{
    this->audio = audio;
}

void StateContext::setGameConfiguration(std::shared_ptr<GameConfiguration> gameConfiguration)
{
    level[0] = gameConfiguration->getStartingLevel();
    level[1] = gameConfiguration->getStartingLevel();

    this->gameConfiguration = gameConfiguration;
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

void StateContext::update() {
    currentState->update();
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