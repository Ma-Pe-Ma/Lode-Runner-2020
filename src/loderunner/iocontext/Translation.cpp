#include "Translation.h"
#include <format>

std::tuple<std::string, float, float> Translation::getTextValue(std::string key, std::string defaultValue, float defaultX, float defaultY) {
	nlohmann::json currentLanguage = translation.value(languages[currentLanguageIndex], nlohmann::json::object());
	nlohmann::json properties = currentLanguage.value(key, nlohmann::json::object());

	std::string value = properties.value("value", defaultValue);

	//TODO: Solve encoding problem for character '©'
	value.erase(std::remove(value.begin(), value.end(), -62), value.end());

	float x = properties.value("x", defaultX);
	float y = properties.value("y", defaultY);

	return std::make_tuple(value, x, y);
};

void Translation::loadGenericTexts() {	
	for (auto it = translation.begin(); it != translation.end(); it++) {
		languages.push_back(it.key());
	}

	if (languages.size() == 0) {
		languages.push_back("English");
	}

	for (currentLanguageIndex = 0; currentLanguageIndex < languages.size(); currentLanguageIndex++) {
		translationMap[currentLanguageIndex] = {};
		translationMap[currentLanguageIndex]["player1"] = getTextValue("player1", "1 PLAYER", 12, 13);
		translationMap[currentLanguageIndex]["player2"] = getTextValue("player2", "2 PLAYERS", 12, 15);
		translationMap[currentLanguageIndex]["editMode"] = getTextValue("editMode", "EDIT MODE", 12, 17);

		translationMap[currentLanguageIndex]["copyRight"] = getTextValue("copyRight", "COPYRIGHT © 1984 HUDSON SOFT", 2, 23);
		translationMap[currentLanguageIndex]["permission"] = getTextValue("permission", "WITH PERMISSION OF", 7, 24);
		translationMap[currentLanguageIndex]["software"] = getTextValue("software", "BRODERBUND SOFTWARE INC", 4, 25);
		translationMap[currentLanguageIndex]["allRights"] = getTextValue("allRights", "ALL RIGHTS RESERVED", 7, 26);

		translationMap[currentLanguageIndex]["cStart"] = getTextValue("cStart", "PRESS START BUTTON", 8, 20);
		translationMap[currentLanguageIndex]["cCopyRight"] = getTextValue("cCopyRight", "COPYRIGHT 1984  DOUG SMITH", 3, 23);
		translationMap[currentLanguageIndex]["cPublish"] = getTextValue("cPublish", "PUBLISHED BY HUDSON SOFT", 4, 24);
		translationMap[currentLanguageIndex]["cLicense"] = getTextValue("cLicense", "UNDER LICENSE FROM", 7, 25);
		translationMap[currentLanguageIndex]["cSoftware"] = getTextValue("cSoftware", "BRODERBUND SOFTWARE INC", 4, 26);

		translationMap[currentLanguageIndex]["playerID"] = getTextValue("playerID", "PLAYER {0}", 12, 6);
		translationMap[currentLanguageIndex]["stage"] = getTextValue("stage", "STAGE {0:03}", 8, 12);
		translationMap[currentLanguageIndex]["life"] = getTextValue("life", "LEFT {0}", 19, 12);
		translationMap[currentLanguageIndex]["score"] = getTextValue("score", "SCORE    {0:08}", 8, 18);
		translationMap[currentLanguageIndex]["hiscore"] = getTextValue("hiscore", "HISCORE  {0:08}", 8, 20);
		translationMap[currentLanguageIndex]["goldPoints"] = getTextValue("goldPoints", "{0} POINTS", 20, 6.5f);
		translationMap[currentLanguageIndex]["enemyPoints"] = getTextValue("enemyPoints", "{0} POINTS", 20, 12.5f);
		translationMap[currentLanguageIndex]["totalPoints"] = getTextValue("totalPoints", "TOTAL {0} POINTS", 14.5f, 23.25f);
		translationMap[currentLanguageIndex]["gameOver"] = getTextValue("gameOver", "GAME OVER", 12, 10);
		translationMap[currentLanguageIndex]["gameOverPlayer"] = getTextValue("gameOverPlayer", "Player", 12.5f, 6);
		translationMap[currentLanguageIndex]["gameTime"] = getTextValue("gameTime", "GAMETIME: {0:.1f} SEC", -5, 0);
	}

	currentLanguageIndex = 0;
}