#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <json.hpp>

#include <memory>

#include "rendering/Text.h"

class Translation {
	nlohmann::json translation;

	int currentLanguageIndex = 0;
	std::vector<std::string> languages;

	std::map<int, std::map<std::string, std::tuple<std::string, float, float>>> translationMap;

	const std::map < std::string, std::tuple<std::string, float, float>> defaultTranslation = {
		{"player1", {"1 PLAYER", 12, 13}},
		{"player2", {"2 PLAYERS", 12, 15}},
		{"editMode", {"EDIT MODE", 12, 17}},

		{"copyRight", {"COPYRIGHT © 1984 HUDSON SOFT", 2, 23}},
		{"permission", {"WITH PERMISSION OF", 7, 24}},
		{"software", {"BRODERBUND SOFTWARE INC", 4, 25}},
		{"allRights", {"ALL RIGHTS RESERVED", 7, 26}},

		{"cStart", {"PRESS START BUTTON", 8, 20}},
		{"cCopyRight", {"COPYRIGHT 1984  DOUG SMITH", 3, 23}},
		{"cPublish", {"PUBLISHED BY HUDSON SOFT", 4, 24}},
		{"cLicense", {"UNDER LICENSE FROM", 7, 25}},
		{"cSoftware", {"BRODERBUND SOFTWARE INC", 4, 26}},

		{"playerID", {"PLAYER {0}", 12, 6}},
		{"stage", {"STAGE {0:03}", 8, 12}},
		{"life", {"LEFT {0}", 19, 12}},
		{"score", {"SCORE    {0:08}", 8, 18}},
		{"hiscore", {"HISCORE  {0:08}", 8, 20}},
		{"goldPoints", {"{0} POINTS", 20, 6.5f}},
		{"bonusPoints", {"{0} POINTS", 20, 9.5f}},
		{"enemyPoints", {"{0} POINTS", 20, 12.5f}},
		{"totalPoints", {"TOTAL {0} POINTS", 14.5f, 23.25f}},
		{"gameOver", {"GAME OVER", 12, 10}},
		{"gameOverPlayer", {"Player {0}", 12.5f, 6}},
		{"gameTime", {"GAMETIME: {0:.1f} SEC   ", -5, 0}},

		{"imguiTitle", {"Lode Runner - configurer", 0, 0}},
		{"gameVersion", {"Game version", 0, 0}},
		{"level", {"Level", 0, 0}},
		{"playerSpeed", {"Player speed", 0, 0}},
		{"enemySpeed", {"Enemy speed", 0, 0}},
		{"language", {"Language", 0, 0}},

		{"controls", {"Controls:", 0, 0}},
		{"arrows", {"arrows - moving", 0, 0}},
		{"leftDig", {"left dig", 0, 0}},
		{"rightDig", {"right dig", 0, 0}},
		{"space", {"previous menu", 0, 0}},
		{"escEnter", {"pause", 0, 0}},
		{"cButton", {"show/hide this window", 0, 0}},

		{"editor", {"Level editor", 0, 0}},
		{"editLevel", {"Edit level", 0, 0}},
		{"edit", {"Edit", 0, 0}},
		{"newText", {"Create new", 0, 0}},
		{"duplicate", {"Duplicate current", 0, 0}},
		{"empty", {"Add empty", 0, 0}},
		{"importText", {"Import levels from file", 0, 0}},
		{"selectFile", {"Select level file", 0, 0}},
		{"clear", {"Clear existing", 0, 0}},
		{"clearTip", {"Delete existing levels before importing", 0, 0}},
		{"import", {"Import", 0, 0}},
		{"exportText", {"Export levels to file", 0, 0}},
		{"export", {"Export", 0, 0}},
		{"builtin", {"Built-in level files:", 0, 0}},
		{"download", {"Download", 0, 0}},
		{"editing", {"Editing: {0}", 0, 0}},
		{"new", {"new", 0, 0}},
		{"save", {"Save", 0, 0}},
		{"cancel", {"Cancel", 0, 0}},
		{"delete", {"Delete", 0, 0}},
	};
public:
	Translation(nlohmann::json translation) {
		this->translation = translation;
	}

	std::tuple<std::string, float, float> getTranslationText(std::string key) {
		return translationMap[currentLanguageIndex][key];
	}

	std::string getTranslationString(std::string key) {
		return std::get<0>(translationMap[currentLanguageIndex][key]);
	}

	int* getCurrentLanguageIndex() {
		return &this->currentLanguageIndex;
	}

	std::vector<std::string> getLanguages() {
		return this->languages;
	}

	void setCurrentLanguageIndex(int currentLanguageIndex) {
		this->currentLanguageIndex = currentLanguageIndex;
	}

	void loadGenericTexts() {
		for (auto it = translation.begin(); it != translation.end(); it++) {
			languages.push_back(it.key());
		}

		if (languages.size() == 0) {
			languages.push_back("English");
		}

		for (currentLanguageIndex = 0; currentLanguageIndex < languages.size(); currentLanguageIndex++) {
			translationMap[currentLanguageIndex] = {};

			nlohmann::json currentLanguage = translation.value(languages[currentLanguageIndex], nlohmann::json::object());

			for (auto it = defaultTranslation.begin(); it != defaultTranslation.end(); it++) {
				nlohmann::json properties = currentLanguage.value(it->first, nlohmann::json::object());

				std::string value = properties.value("value", std::get<0>(it->second));

				//TODO: Solve encoding problem for character '�'
				value.erase(std::remove(value.begin(), value.end(), -62), value.end());

				float x = properties.value("x", std::get<1>(it->second));
				float y = properties.value("y", std::get<2>(it->second));

				translationMap[currentLanguageIndex][it->first] = std::make_tuple(value, x, y);
			}
		}

		currentLanguageIndex = 0;
	}
};

#endif