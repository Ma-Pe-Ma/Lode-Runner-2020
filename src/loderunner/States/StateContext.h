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

class StateContext {
private:
	State* currentState;
	void transitionTo(State*, bool start = true, bool end = true);

	State* transitionableAtEndOfFrame = nullptr;

	MainMenu* mainMenu;
	Intro* intro;
	GamePlay* gamePlay;
	Select* select;
	Outro* outro;
	GameOver* gameOver;
	Generator* generator;

	std::shared_ptr<RenderingManager> renderingManager;
	std::shared_ptr<IOContext> ioContext;
	std::shared_ptr<GameConfiguration> gameConfiguration;
	std::shared_ptr<Audio> audio;

public:	
	void transitionToAtEndOfFrame(State*, bool start = true, bool end = true);
	void checkTransitionAtEndofFrame();

	StateContext();
	void initialize();
	
	void update(float);

	//cursor of the menu
	int menuCursor = 0;
	int level[2] = { 1,1 };
	int playerLife[2] = { 5,5 };
	int score[2] = { 0,0 };

	int playerNr = 0;
	int highScore = 0;

	std::shared_ptr<RenderingManager> getRenderingManager() { return this->renderingManager; }
	std::shared_ptr<IOContext> getIOContext() { return this->ioContext; }
	std::shared_ptr<GameConfiguration> getGameConfiguration() { return this->gameConfiguration; }
	std::shared_ptr<Audio> getAudio() { return this->audio; }

	void setRenderingManager(std::shared_ptr<RenderingManager>);
	void setIOContext(std::shared_ptr<IOContext>);
	void setGameConfiguration(std::shared_ptr<GameConfiguration>);
	void setAudio(std::shared_ptr<Audio>);

	void setMainMenu(MainMenu* mainMenu)
	{
		this->mainMenu = mainMenu;
	}

	MainMenu* getMainMenu()
	{
		return this->mainMenu;
	}

	void setIntro(Intro* intro)
	{
		this->intro = intro;
	}

	Intro* getIntro()
	{
		return this->intro;
	}

	void setGamePlay(GamePlay* gamePlay)
	{
		this->gamePlay = gamePlay;
	}

	GamePlay* getGamePlay()
	{
		return this->gamePlay;
	}

	void setSelect(Select* select)
	{
		this->select= select;
	}

	Select* getSelect()
	{
		return this->select;
	}

	void setOutro(Outro* outro)
	{
		this->outro = outro;
	}

	Outro* getOutro()
	{
		return this->outro;
	}

	void setGameOver(GameOver* gameOver)
	{
		this->gameOver = gameOver;
	}

	GameOver* getGameOver()
	{
		return this->gameOver;
	}

	void setGenerator(Generator* generator)
	{
		this->generator = generator;
	}

	Generator* getGenerator()
	{
		return this->generator;
	}
};

#endif