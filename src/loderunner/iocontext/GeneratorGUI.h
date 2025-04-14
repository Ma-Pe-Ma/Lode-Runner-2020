#ifndef GENERATORGUI_H
#define GENERATORGUI_H

#include <string>
#include <imgui/imgui.h>
#include <json.hpp>
#include <optional>

class Generator;

enum class GeneratorState {
	select,
	edit
};

class GeneratorGUI {
	nlohmann::json generatorLevels;

	Generator* generator;
	GeneratorState generatorState = GeneratorState::select;

	std::optional<int> cursor;

	const std::map<short, char> characterMap = {
		{0, ' '},
		{1, '#'},
		{2, '@'},
		{3, 'H'},
		{4, '-'},
		{5, 'X'},
		{6, 'S'},
		{7, '$'},
		{8, '0'},
		{9, '&'}
	};

	void loadLevel();

	void select();
	void edit();
public:
	GeneratorGUI(Generator* generator);

	void start();
	void update();

	GeneratorState getGeneratorState() { return this->generatorState; }
};

#endif