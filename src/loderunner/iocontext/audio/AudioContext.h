#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <memory>
#include <string>

#include "AudioStatus.h"
#include "AudioFile.h"

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMedia.h"
#endif

class AudioFile;

#define FRAMES_PER_BUFFER 64
#define CHANNEL_COUNT 2
#define SAMPLE_RATE 44100

class AudioContext {

private:

protected:
#ifdef VIDEO_RECORDING
	std::shared_ptr<MultiMedia> multiMedia;
#endif
	std::vector<std::shared_ptr<AudioFile>> audioFiles;
	
	std::string folderPath;
	std::vector<std::string> audioFileNames;

	//std::map<std::string, std::shared_ptr<AudioFile>> fileMap;
public:
	virtual void initialize() = 0;
	virtual void terminate() = 0;

	void setAudioFileNames(std::string folderPath, std::vector<std::string> audioFileNames) { 
		this->folderPath = folderPath;
		this->audioFileNames = audioFileNames;
	}

	virtual void openAudioFiles() {
		for (auto iterator = audioFileNames.begin(); iterator != audioFileNames.end(); iterator++) {
			std::string audioFileLocation = folderPath + *iterator + ".ogg";
			audioFiles.push_back(std::make_shared<AudioFile>(audioFileLocation));
		}
	}

	void closeAudioFiles() {
		for (auto iterator = audioFiles.begin(); iterator != audioFiles.end(); iterator++) {
			(*iterator)->closeSoundFile();
		}
	}
	
	std::shared_ptr<AudioFile> getAudioFileByID(int n) {
		return audioFiles[n];
	}

#ifdef VIDEO_RECORDING
	void setMultiMedia(std::shared_ptr<MultiMedia> multiMedia) {
		this->multiMedia = multiMedia;
	}
#endif
};
#endif // !AUDIO_H
