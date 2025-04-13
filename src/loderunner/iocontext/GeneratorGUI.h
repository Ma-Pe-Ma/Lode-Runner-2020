#ifndef GENERATORGUI_H
#define GENERATORGUI_H

#include <string>
#include <imgui/imgui.h>
#include <json.hpp>

class Generator;

enum class GeneratorState {
	select,
	edit
};

class GeneratorGUI {
	nlohmann::json generatorLevels;

	Generator* generator;
	GeneratorState generatorState = GeneratorState::select;
public:
	GeneratorGUI(Generator* generator) {
		this->generator = generator;
	}

	void start();
	void update();

	void select();
	void edit();

	GeneratorState getGeneratorState() { return this->generatorState; }
};

#endif