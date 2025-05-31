#ifndef STATECONTEXT_H
#define STATECONTEXT_H

#include <array>

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
	std::shared_ptr<AudioContext> audio;

	void checkTransitionAtEndOfFrame();

	//cursor of the menu
	short menuCursor = 0;
	std::array<unsigned int, 2> playerLife = { 5,5 };
	std::array<unsigned int, 2> score = { 0,0 };

	short playerNr = 0;
	short highScore = 0;

	bool showImGuiWindow = true;
public:	
	StateContext();

	void initialize();	
	void update();	

	void transitionToAtEndOfFrame(State*, bool start = true, bool end = true);

	std::shared_ptr<RenderingManager> getRenderingManager() { return this->renderingManager; }
	std::shared_ptr<IOContext> getIOContext() { return this->ioContext; }
	std::shared_ptr<GameConfiguration> getGameConfiguration() { return this->gameConfiguration; }
	std::shared_ptr<AudioContext> getAudio() { return this->audio; }

	void setRenderingManager(std::shared_ptr<RenderingManager>);
	void setIOContext(std::shared_ptr<IOContext>);
	void setGameConfiguration(std::shared_ptr<GameConfiguration>);
	void setAudio(std::shared_ptr<AudioContext>);

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

	State* getCurrentState()
	{
		return this->currentState;
	}

	void setMenuCursor(int menuCursor) { this->menuCursor = menuCursor; }
	short& getMenuCursor() { return this->menuCursor; }

	void setPlayerNr(short playerNr) { this->playerNr = playerNr; }
	short& getPlayerNr() { return this->playerNr; }

	std::array<unsigned int, 2>& getPlayerLife() { return playerLife; }
	std::array<unsigned int, 2>& getPlayerScore() { return score; }

	short& getHighScore() { return this->highScore; }

	int& getCurrentLevel() {
		return this->getGameConfiguration()->getLevel()[getPlayerNr()];
	}

	bool& getShowImGuiWindow() { return showImGuiWindow; }
};

#endif