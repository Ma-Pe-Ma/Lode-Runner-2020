#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <memory>
#include <string>

#include "AudioStatus.h"

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMedia.h"
#endif

class AudioFile;

#define FRAMES_PER_BUFFER 64
#define CHANNEL_COUNT 2
#define SAMPLE_RATE 44100

class Audio {

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
	void setAudioFileNames(std::string folderPath, std::vector<std::string> audioFileNames) { 
		this->folderPath = folderPath;
		this->audioFileNames = audioFileNames;
	}

	virtual void initialize() = 0;
	virtual void terminate() = 0;

	virtual void openAudioFiles();
    void closeAudioFiles();
	
	std::shared_ptr<AudioFile> getAudioFileByID(int n)
	{
		return audioFiles[n];
	}
};
#endif // !AUDIO_H
