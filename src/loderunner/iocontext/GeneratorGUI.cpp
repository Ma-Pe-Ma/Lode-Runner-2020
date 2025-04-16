#include "GeneratorGUI.h"
#include "../states/Generator.h"
#include "../states/StateContext.h"

#ifdef __EMSCRIPTEN__
#include "EmscriptenIOContext.h"
#endif

GeneratorGUI::GeneratorGUI(Generator* generator) {
	this->generator = generator;
}

void GeneratorGUI::start() {
	if (generatorLevels.is_null()) {
		auto mapLevels = this->generator->getStateContext()->getIOContext()->loadGeneratorLevels();

		generatorLevels = nlohmann::json::array();

		nlohmann::json orderedLevels;

		int index = 0;
		int newIndex = 0;
		for (auto mapIt = mapLevels.begin(); mapIt != mapLevels.end(); ++mapIt) {
			if (mapLevels.contains(std::to_string(index))) {
				orderedLevels[std::to_string(newIndex++)] = mapLevels[std::to_string(index)];
			}
			index++;			
		}

		for (int index = 0; index < orderedLevels.size(); ++index) {
			generatorLevels.push_back(orderedLevels[std::to_string(index)]);
		}

		if (generatorLevels.size() > 0) {
			cursor.emplace(0);
		}
	}

#ifdef __EMSCRIPTEN__
	(std::static_pointer_cast<EmscriptenIOContext>(generator->getStateContext()->getIOContext()))->clearRawImportableLevels();
#endif

	if (generatorState == GeneratorState::select && cursor.has_value()) {
		loadLevel();
	}
}

void GeneratorGUI::update() {
#ifndef __EMSCRIPTEN__
	ImVec2 windowSize = ImVec2(220, 115);
#else
	ImVec2 windowSize = ImVec2(220, 265);
#endif
	auto screenSize = generator->getStateContext()->getIOContext()->getScreenParameters().screenSize;
	ImGui::SetNextWindowPos(ImVec2(17 * std::get<0>(screenSize) / 25, std::get<0>(screenSize) / 25), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(windowSize);

	ImGui::Begin("Level editor", nullptr, ImGuiWindowFlags_NoResize);

	switch (generatorState) {
	case GeneratorState::select:
		this->select();
		break;
	case GeneratorState::edit:
		this->edit();
		break;	

#ifdef __EMSCRIPTEN__
	case GeneratorState::import:
		this->importLevels();
		this->select();
		break;
	default:
		
		break;
#endif
	}

	ImGui::End();
}

void GeneratorGUI::select() {
	if (generatorLevels.size() == 0) {
		ImGui::BeginDisabled();
		cursor.reset();
	}

	ImGui::Text("Edit level");

	ImGui::Indent(10.0f);

	if (generatorLevels.size() > 9) {
		if (ImGui::Button("<<")) {
			cursor.emplace(cursor.value() - 10 < 0 ? 0 : cursor.value() - 10);
			loadLevel();
		}

		ImGui::SameLine();
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

	if (generatorLevels.size() > 9) {
		ImGui::SameLine();
		if (ImGui::Button(">>")) {
			cursor.emplace(cursor.value() + 10 >= generatorLevels.size() ? generatorLevels.size() - 1 : cursor.value() + 10);
			loadLevel();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Edit")) {
		this->generatorState = GeneratorState::edit;
	}

	ImGui::Unindent(10.0f);

	ImGui::Text("Create new");
	
	ImGui::Indent(10.0f);

	if (ImGui::Button("Duplicate current")) {
		this->generatorState = GeneratorState::edit;
		cursor.reset();
	}

	if (generatorLevels.size() == 0) {
		ImGui::EndDisabled();
	}

	ImGui::SameLine();

	if (ImGui::Button("Add empty")) {
		cursor.reset();
		this->generator->setGeneratedLayout();

		this->generatorState = GeneratorState::edit;
	}

	ImGui::Unindent(10.0f);

#ifdef __EMSCRIPTEN__

	std::shared_ptr<EmscriptenIOContext> ioContext = std::static_pointer_cast<EmscriptenIOContext>(generator->getStateContext()->getIOContext());

	ImGui::Text("Import levels from file");

	ImGui::Indent(10.0f);

	if (ImGui::Button("Select level file")) {
		EmscriptenHandler::openLevelFilePicker();
	}

	if (!ioContext->getRawImportableLevels().has_value()) {
		ImGui::BeginDisabled();
	}

	ImGui::SameLine();
	std::string fileName = ioContext->getRawImportableLevels().has_value() ? std::get<0>(ioContext->getRawImportableLevels().value()) : "-";

	ImGui::Text(fileName.c_str());

	ImGui::Checkbox("Clear existing", &clearExistingWhenImport);

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Delete existing levels before importing");
	}

	ImGui::SameLine();

	if (ImGui::Button("Import")) {
		this->generatorState = GeneratorState::import;
	}

	if (!ioContext->getRawImportableLevels().has_value()) {
		ImGui::EndDisabled();
	}

	if (importExportMessage.has_value()) {
		ImGui::Text(importExportMessage.value().c_str());
	}

	ImGui::Unindent(10.0f);

	ImGui::Text("Export levels to file");

	ImGui::Indent(10.0f);

	if (ImGui::Button("Export")) {
		nlohmann::json exportable;

		for (int i = 0; i < generatorLevels.size(); ++i) {
			exportable[std::to_string(i)] = generatorLevels[i];
		}

		ioContext->exportLevels(exportable);
	}
	ImGui::Unindent(10.0f);
	
	ImGui::Text("Built-in level files:");
	ImGui::SameLine();
	ImGui::TextLinkOpenURL("Download", LEVEL_URL);
#endif
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
			generatorLevels.push_back(levelJSON);
			cursor.emplace(generatorLevels.size() - 1);
		}
		else {
			generatorLevels[cursor.value()] = levelJSON;
		}

		saveLevels();

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
				generatorLevels[i] = generatorLevels[i + 1];
			}

			generatorLevels.erase(generatorLevels.size() - 1);

			saveLevels();

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
	nlohmann::json levelJSON = generatorLevels[cursor.value()];

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

void GeneratorGUI::saveLevels() {
	nlohmann::json saveableJson;
	for (int i = 0; i < generatorLevels.size(); ++i) {
		saveableJson[std::to_string(i)] = generatorLevels[i];
	}
	this->generator->getStateContext()->getIOContext()->saveGeneratorLevels(saveableJson);
}


#ifdef __EMSCRIPTEN__
void GeneratorGUI::importLevels() {
	std::shared_ptr<EmscriptenIOContext> ioContext = std::static_pointer_cast<EmscriptenIOContext>(generator->getStateContext()->getIOContext());

	try {
		std::string rawLevels = std::get<1>(ioContext->getRawImportableLevels().value());
		nlohmann::json newLevels = nlohmann::json::parse(rawLevels);
		nlohmann::json orderedLevels;

		//transforming from key-level[newLevels], to properly ordered array[orderedLevels]
		int index = 0;
		int newIndex = 0;
		for (auto mapIt = newLevels.begin(); mapIt != newLevels.end(); ++mapIt) {
			if (newLevels.contains(std::to_string(index))) {
				orderedLevels[std::to_string(newIndex++)] = newLevels[std::to_string(index)];
			}
			index++;			
		}

		for (int index = 0; index < orderedLevels.size(); ++index) {
			std::string key = std::to_string(index);
			nlohmann::json& level = orderedLevels[key];

			if (!level.is_array()) {
				std::string error = "Level-{0}:\n\tis not an array!";
				throw InvalidLevelException(std::vformat(error, std::make_format_args(key)));
			}

			if (level.size() != 16) {
				std::string error = "Level-{0}:\n\tdoes not contain 16 lines!";
				throw InvalidLevelException(std::vformat(error, std::make_format_args(key)));
			}

			int rowIndex = 0;
			for (auto rowIt = level.begin(); rowIt != level.end(); ++rowIt) {
				if (!rowIt->is_string()) {
					std::string error = "Level-{0}, row-{1}: \n\tis not a string!";
					throw InvalidLevelException(std::vformat(error, std::make_format_args(key, rowIndex)));
				}

				std::string row = rowIt.value().get<std::string>();

				if (row.length() != 28) {
					std::string error = "Level-{0}, row-{1}: \n\tdoes not contain 28 characters!";
					throw InvalidLevelException(std::vformat(error, std::make_format_args(key, rowIndex)));
				}

				int columnIndex = 0;
				for (const auto& c : row) {
					bool characterExists = false;
					for (const auto& pair : characterMap) {
						if (pair.second == c) {
							characterExists = true;
							break;
						}
					}

					if (!characterExists) {
						std::string error = "Level-{0}, row-{1}, column-{2}:\n\tinvalid character: {3}.";
						throw InvalidLevelException(std::vformat(error, std::make_format_args(key, rowIndex, columnIndex, c)));
					}
					++columnIndex;
				}

				++rowIndex;
			}
		}

		if (clearExistingWhenImport) {
			generatorLevels.clear();
			clearExistingWhenImport = false;
		}

		for (int i = 0; i < orderedLevels.size(); i++) {
			generatorLevels.push_back(orderedLevels[std::to_string(i)]);
		}

		std::string levelFileName = std::get<0>(ioContext->getRawImportableLevels().value());
		importExportMessage.emplace(std::vformat("Importing file '{0}'\nwas successful.", std::make_format_args(levelFileName)));
		ioContext->clearRawImportableLevels();

		saveLevels();

		if (generatorLevels.size() == 0) {
			cursor.reset();
		}
		else {
			cursor.emplace(0);
			loadLevel();
		}
	}
	catch (const nlohmann::json::exception& ex) {
		importExportMessage.emplace("Invalid JSON File");
	}
	catch (const InvalidLevelException& ex) {
		importExportMessage.emplace(ex.what());
	}

	this->generatorState = GeneratorState::select;
}
#endif