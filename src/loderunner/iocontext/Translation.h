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

	std::tuple<std::string, float, float> getTextValue(std::string key, std::string defaultValue, float defaultX, float defaultY);

public:
	Translation(nlohmann::json translation) {
		this->translation = translation;
	}

	std::array<std::shared_ptr<std::vector<std::shared_ptr<Text>>>, 2> loadMainMenuTexts();
	void loadGenericTexts();

	std::tuple<std::string, float, float> getTranslationText(std::string key) {
		return translationMap[currentLanguageIndex][key];
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
};

#endif