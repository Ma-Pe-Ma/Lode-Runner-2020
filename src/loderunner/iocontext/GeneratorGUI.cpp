#include "GeneratorGUI.h"
#include "../states/Generator.h"
#include "../states/StateContext.h"

GeneratorGUI::GeneratorGUI(Generator* generator) {
	this->generator = generator;
}

void GeneratorGUI::start() {
	if (generatorLevels.is_null()) {
		generatorLevels = this->generator->getStateContext()->getIOContext()->loadGeneratorLevels();

		if (generatorLevels.size() > 0) {
			cursor.emplace(0);
		}
	}

	if (generatorState == GeneratorState::select && cursor.has_value()) {
		loadLevel();
	}
}

void GeneratorGUI::update() {
	ImVec2 windowSize = ImVec2(220, 100);
	auto screenSize = generator->getStateContext()->getIOContext()->getScreenParameters().screenSize;
	ImGui::SetNextWindowPos(ImVec2(std::get<0>(screenSize) / 25, std::get<0>(screenSize) / 25), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(windowSize);

	ImGui::Begin("Generator", nullptr, ImGuiWindowFlags_NoResize);

	switch (generatorState) {
	case GeneratorState::select:
		this->select();
		break;
	case GeneratorState::edit:
		this->edit();
		break;
	}

	ImGui::End();
}

void GeneratorGUI::select() {
	if (generatorLevels.size() == 0) {
		ImGui::BeginDisabled();
		cursor.reset();
	}

	if (ImGui::Button("<")) {
		cursor.emplace(cursor.value() - 1 < 0 ? 0 : cursor.value() - 1);
		loadLevel();
	}

	ImGui::SameLine();

	std::string cursorString = cursor.has_value() ? std::to_string(cursor.value() + 1) + "/" + std::to_string(generatorLevels.size()) : "-";
	ImGui::Text(cursorString.c_str());

	ImGui::SameLine();

	if (ImGui::Button(">")) {
		cursor.emplace(cursor.value() + 1 == generatorLevels.size() ? generatorLevels.size() - 1 : cursor.value() + 1);
		loadLevel();
	}

	ImGui::SameLine();

	if (ImGui::Button("Edit")) {
		this->generatorState = GeneratorState::edit;
	}

	if (ImGui::Button("Duplicate")) {
		this->generatorState = GeneratorState::edit;
		cursor.reset();
	}

	if (generatorLevels.size() == 0) {
		ImGui::EndDisabled();
	}

	ImGui::SameLine();

	if (ImGui::Button("Create empty")) {
		cursor.reset();
		this->generator->setGeneratedLayout();

		this->generatorState = GeneratorState::edit;
	}
}

void GeneratorGUI::edit() {
	std::string editableString = cursor.has_value() ? "Editing: " + std::to_string(cursor.value() + 1) : "Editing: new";

	ImGui::Text(editableString.c_str());

	if (ImGui::Button("Save")) {
		std::array<std::array<short, 28>, 16> currentLayout = generator->getGeneratedLayout();

		nlohmann::json levelJSON;

		for (int j = 0; j < 16; j++) {
			std::stringstream row;

			for (int i = 0; i < 28; i++) {
				row << characterMap.at(currentLayout[15 - j][i]);
			}

			levelJSON.push_back(row.str());		
			row.clear();
		}		

		if (!cursor.has_value()) {
			generatorLevels[std::to_string(generatorLevels.size())] = levelJSON;
			cursor.emplace(generatorLevels.size() - 1);
		}
		else {
			generatorLevels[std::to_string(cursor.value())] = levelJSON;
		}

		this->generator->getStateContext()->getIOContext()->saveGeneratorLevels(generatorLevels);

		loadLevel();
		this->generatorState = GeneratorState::select;
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		this->generatorState = GeneratorState::select;

		if (!cursor.has_value()) {
			if (generatorLevels.size() > 0) {
				cursor.emplace(0);
				loadLevel();
			}
			else {
				generator->setGeneratedLayout();
			}
		}
		else {
			loadLevel();
		}
	}

	if (cursor.has_value()) {
		ImGui::SameLine();
		if (ImGui::Button("Delete")) {
			for (int i = cursor.value(); i < generatorLevels.size() - 1; i++) {
				generatorLevels[std::to_string(i)] = generatorLevels[std::to_string(i + 1)];
			}

			generatorLevels.erase(std::to_string(generatorLevels.size() - 1));

			this->generator->getStateContext()->getIOContext()->saveGeneratorLevels(generatorLevels);

			if (generatorLevels.size() > 0) {
				if (cursor.value() != 0) {
					cursor.emplace(cursor.value() - 1);
				}

				loadLevel();
			}
			else {
				generator->setGeneratedLayout();
				cursor.reset();
			}

			this->generatorState = GeneratorState::select;
		}
	}	
}

void GeneratorGUI::loadLevel() {
	nlohmann::json levelJSON = generatorLevels[std::to_string(cursor.value())];

	std::array<std::array<short, 28>, 16> loadedLevel = {};

	for (int j = 0; j < 16; j++) {
		std::string row = levelJSON[j].get<std::string>();

		for (int i = 0; i < 28; i++) {
			for (const auto& pair : characterMap) {
				if (pair.second == row[i]) {
					loadedLevel[15 - j][i] = pair.first;
				}
			}
		}
	}

	this->generator->setGeneratedLayout(loadedLevel);
}