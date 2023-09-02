#include "Audio.h"

#include "AudioFile.h"

void Audio::openAudioFiles() {
	for (auto iterator = audioFileNames.begin(); iterator != audioFileNames.end(); iterator++)
	{
		std::string audioFileLocation = folderPath + *iterator + ".ogg";
		audioFiles.push_back(std::make_shared<AudioFile>(audioFileLocation));
	}
}

void Audio::closeAudioFiles() {
	for (auto iterator = audioFiles.begin(); iterator != audioFiles.end(); iterator++)
	{
		(*iterator)->closeSoundFile();
	}
}