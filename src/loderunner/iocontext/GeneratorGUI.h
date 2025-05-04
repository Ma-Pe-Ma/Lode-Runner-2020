#ifndef GENERATORGUI_H
#define GENERATORGUI_H

#include <string>
#include <imgui/imgui.h>
#include <json.hpp>
#include <optional>

class Generator;
class Translation;

enum class GeneratorState {
	select = 0,
	edit = 1,
	import = 2
};

class InvalidLevelException : public std::exception {
	std::string message;
public:
	InvalidLevelException(std::string message) {
		this->message = message;
	}

	const char* what() const noexcept override {
		return this->message.c_str();
	}
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
	void saveLevels();

	void select();
	void edit();

	std::shared_ptr<Translation> translation;
#ifdef __EMSCRIPTEN__
	bool clearExistingWhenImport = false;
	void importLevels();
	std::optional<std::string> importExportMessage;
#endif
public:
	GeneratorGUI(Generator* generator);

	void start();
	void update();

	GeneratorState getGeneratorState() { return this->generatorState; }
};

#endif