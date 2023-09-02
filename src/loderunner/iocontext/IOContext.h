#ifndef IOCONTEXT_H
#define IOCONTEXT_H

#include "../Button.h"
#include <tuple>
#include <memory>
#include <string>
#include <functional>

class GameConfiguration;

class IOContext {

protected:
	Button up, down, leftButton, rightButton, space, enter, leftDigButton, rightDigButton, configButton, pauseButton;

#ifndef NDEBUG
	Button fButton, gButton, hButton, tButton, jButton, kButton, lButton, iButton;
#endif

	std::tuple<unsigned int, unsigned int> screenSize = std::make_tuple(1100, 600);
	std::tuple<unsigned int, unsigned int> screenPosition = std::make_tuple(30, 30);
	std::tuple<unsigned int, unsigned int> formerScreenSize = std::make_tuple(1500, 900);

	std::tuple<unsigned int, unsigned int> viewPortPosition = std::make_tuple(0, 0);
	std::tuple<unsigned int, unsigned int> viewPortSize = std::make_tuple(1100, 600);
public:
	virtual void processInput() = 0;

	virtual unsigned int loadTexture(char const* path) = 0;
	virtual void takeScreenShot() = 0;
	virtual void loadConfig(std::shared_ptr<GameConfiguration>) = 0;

	virtual void initialize() = 0;

	virtual void loadLevel(std::string, std::function<bool(std::string)>) = 0;

	std::tuple<unsigned int, unsigned int> getScreenSize() { return this->screenSize; }
	std::tuple<unsigned int, unsigned int> getScreenPosition() { return this->screenPosition; }

	std::tuple<unsigned int, unsigned int> getViewPortSize() { return this->viewPortSize; }
	std::tuple<unsigned int, unsigned int> getViewPortPosition() { return this->viewPortPosition; }

	void updateViewPortValues(int width, int height) {
		screenSize = std::make_tuple(width, height);

		float screenRatio = ((float) width) / height;
		  
		if (screenRatio >= 15.0f / 9) {
			int viewPortHeight = height;
			viewPortHeight -= viewPortHeight % 2;

			int viewPortWidth = 15.0f / 9 * height;
			viewPortWidth -= viewPortWidth % 2;

			viewPortSize = std::make_tuple(viewPortWidth, viewPortHeight);
			viewPortPosition = std::make_tuple((width - viewPortWidth) / 2, 0);
		}
		else {
			int viewPortWidth = width;
			viewPortWidth -= viewPortWidth % 2;

			int viewPortHeight = 9.0f / 15 * viewPortWidth;
			viewPortHeight -= viewPortHeight % 2;

			viewPortSize = std::make_tuple(viewPortWidth, viewPortHeight);
			viewPortPosition = std::make_tuple(0, (height - viewPortHeight) / 2);
		}
	}

	Button& getUpButton() { return up; }
	Button& getDownButton() { return down; }
	Button& getLeftButton() { return leftButton; }
	Button& getRightButton() { return rightButton; }

	Button& getSpaceButton() { return space; }
	Button& getEnterButton() { return enter; }
	Button& getPauseButton() { return pauseButton; }

	Button& getLeftDigButton() { return leftDigButton; }
	Button& getRightDigButton() { return rightDigButton; }

	Button& getConfigButton() { return configButton; }

	virtual void saveConfig(std::string key, std::string value) = 0;

#ifndef NDEBUG
	Button& getFButton() { return fButton; }
	Button& getGButton() { return gButton; }
	Button& getHButton() { return hButton; }
	Button& getTButton() { return tButton; }

	Button& getJButton() { return jButton; }
	Button& getKButton() { return kButton; }
	Button& getLButton() { return lButton; }
	Button& getIButton() { return iButton; }
#endif
};

#endif