#include "configreader.h"

#include <fstream>
#include <map>
#include <string>
#include <QDebug>
#include <QApplication>

namespace ConfigReader {
    int resolutionMode = 0;
    bool levelset = false;
    bool usCover = false;

    float enemySpeed = 0.415f;
    float playerSpeed = 0.9f;

    int width = 1280;
    int height = 720;

    int fps = 60;
    int recordingHeight = 720;
    int startingLevel = 1;

    std::map<std::string, std::string> configMap;

    int getIntByKey(std::string key, int defaultValue) {
        std::string value = configMap[key];

        if (value != "") {
            try {
                return std::stoi(value);
            }
            catch (std::invalid_argument e) {
                return defaultValue;
            }
        }

        return defaultValue;
    }

    float getFloatByKey(std::string key, float defaultValue) {
        std::string value = configMap[key];

        if (value != "") {
            try {
                return std::stof(value);
            }
            catch (std::invalid_argument e) {
                return defaultValue;
            }
        }

        return defaultValue;
    }

    void read() {
        QString path = QCoreApplication::applicationDirPath()+"/config.txt";
        std::ifstream config(path.toUtf8().constData(), std::fstream::in);
        std::string line;

        if (config.is_open()) {
            while (getline(config, line)) {
                if (line.length() == 0 || line[0] == '#') {
                    continue;
                }

                std::string key = line.substr(0, line.find(' '));
                std::string value = line.substr(line.find(' ') + 1);

                configMap[key] = value;
            }
         }
        else {
           qInfo()<<"File not open";
        }

        config.close();

        resolutionMode = getIntByKey("resolution", resolutionMode);
        levelset = getIntByKey("levelset", levelset);
        startingLevel = getIntByKey("levelNr", startingLevel);
        usCover = getIntByKey("USCOVER", usCover);
        width = getIntByKey("width", width);
        height = getIntByKey("height", height);
        enemySpeed = getFloatByKey("enemySpeed", enemySpeed);
        playerSpeed = getFloatByKey("playerSpeed", playerSpeed);
        fps = getIntByKey("FPS", fps);
        recordingHeight = getIntByKey("RecordingHeight", recordingHeight);
    }

    std::ofstream configOut;

    void writeEntry(std::string explanation, std::string value) {
        explanation += "\n";
        value += "\n";

        configOut.write(explanation.c_str(), explanation.size());
        configOut.write(value.c_str(), value.size());
        configOut.write("\n", 1);
    }

    bool write() {
        QString path = QCoreApplication::applicationDirPath()+"/config.txt";
        configOut = std::ofstream(path.toUtf8().constData(), std::fstream::out);

        //resolution
        std::string resolutionExplanation = "# given resolutions: 0=1500×900, 1=750×450, 2=3000×1800, 3=1750×1050, 4=custom";
        std::string resolutionString = "resolution " + std::to_string(resolutionMode);
        writeEntry(resolutionExplanation, resolutionString);

        //customResolution
        std::string customResolutionExplanation = "# custom resolution\n";
        configOut.write(customResolutionExplanation.c_str(), customResolutionExplanation.size());
        std::string widthString = "width " + std::to_string(width) + "\n";
        configOut.write(widthString.c_str(), widthString.size());
        std::string heightString = "height " + std::to_string(height) + "\n";
        configOut.write(heightString.c_str(), heightString.size());
        configOut.write("\n", 1);

        //levelset
        std::string levelsetExplanation = "# levelset: 0 = original levels, 1 = Championship LR levels";
        std::string levelsetString = "levelset " + std::to_string(levelset);
        writeEntry(levelsetExplanation, levelsetString);

        //startingLevel
        std::string startingLevelExplanation = "# jumping to level (only at startup) (original levels: 1-150, championship levels: 1-51)";
        std::string startingLevelString = "levelNr " + std::to_string(startingLevel);
        writeEntry(startingLevelExplanation, startingLevelString);

        //US Cover
        std::string coverExplanation = "# setting US cover for main menu: 0 - false, other values - true";
        std::string coverString = "USCOVER " + std::to_string(usCover);
        writeEntry(coverExplanation, coverString);

        //fps
        std::string fpsExplanation = "# Set framerate, please note that currently the video recording only works with 60 fps";
        std::string fpsString = "FPS " + std::to_string(fps);
        writeEntry(fpsExplanation, fpsString);

        //recordingheight
        std::string recordingExplanation = "# downscaling video height (and proportionally width also) to this value (only when the window's height is bigger than this value)";
        std::string recordingString = "RecordingHeight " + std::to_string(recordingHeight);
        writeEntry(recordingExplanation, recordingString);

        //playerSpeed
        std::string playerExplanation = "# speed of the runner";
        std::string playerString = "playerSpeed " + std::to_string(playerSpeed);
        writeEntry(playerExplanation, playerString);

        //enemySpeed
        std::string enemyExplanation = "# speed of the guards";
        std::string enemyString = "enemySpeed " + std::to_string(enemySpeed);
        writeEntry(enemyExplanation, enemyString);

        configOut.close();
        return true;
    }
}
