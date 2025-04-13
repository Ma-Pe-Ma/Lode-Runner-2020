#include "GeneratorGUI.h"
#include "../states/Generator.h"
#include "../states/StateContext.h"

void GeneratorGUI::start() {
	std::string filePath = "./resources/generator.json";

	std::ifstream fileStream;
	fileStream.open(filePath);

	if (fileStream.fail()) {
		std::cout << "Generator not exist!" << std::endl;
		std::ofstream newFile;
		newFile.open(filePath);
		newFile << "{}";
	}

	fileStream.open(filePath);

	generatorLevels = nlohmann::json::parse(fileStream);
}

void GeneratorGUI::update() {
	ImVec2 windowSize = ImVec2(220, 395);
	auto screenSize = generator->getStateContext()->getIOContext()->getScreenParameters().screenSize;
	ImGui::SetNextWindowPos(ImVec2(20 * std::get<0>(screenSize) / 25, std::get<0>(screenSize) / 25), ImGuiCond_FirstUseEver);
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
	if (generatorLevels.size()) {
		ImGui::BeginDisabled();
	}

	if (ImGui::Button("<")) {

	}

	ImGui::SameLine();

	ImGui::Text("?");

	ImGui::SameLine();

	if (ImGui::Button(">")) {

	}

	if (ImGui::Button("Edit")) {
		this->generatorState = GeneratorState::edit;
	}

	ImGui::SameLine();

	if (generatorLevels.size()) {
		ImGui::EndDisabled();
	}

	ImGui::SameLine();

	if (ImGui::Button("Create new")) {
		this->generatorState = GeneratorState::edit;
	}
}

void GeneratorGUI::edit() {
	if (ImGui::Button("Save")) {
		this->generatorState = GeneratorState::select;
	}

	if (ImGui::Button("Discard")) {
		this->generatorState = GeneratorState::select;
	}
}