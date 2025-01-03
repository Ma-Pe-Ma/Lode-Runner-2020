#ifndef SCREENPARAMETERS_H
#define SCREENPARAMETERS_H

#include <tuple>

struct ScreenParameters {
	std::tuple<unsigned int, unsigned int> screenSize = std::make_tuple(1100, 600);
	std::tuple<unsigned int, unsigned int> screenPosition = std::make_tuple(30, 30);
	std::tuple<unsigned int, unsigned int> formerScreenSize = std::make_tuple(1500, 900);

	std::tuple<unsigned int, unsigned int> viewPortPosition = std::make_tuple(0, 0);
	std::tuple<unsigned int, unsigned int> viewPortSize = std::make_tuple(1100, 600);

	void updateViewPortValues(int width, int height) {
		screenSize = std::make_tuple(width, height);

		float screenRatio = ((float)width) / height;

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
};

#endif