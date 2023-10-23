#ifndef OPENALAUDIO_H
#define OPENALAUDIO_H

#include "AudioContext.h"

#include "OpenALAudioFile.h"

#include <AL/al.h>
#include <AL/alc.h>

class OpenALAudio : public AudioContext {
	ALCdevice* device;
	ALCcontext* openALContext;

public:
	void initialize() override {
		device = alcOpenDevice(NULL);
		if (!device) {
			std::cout << "DEVICE NULL" << std::endl;
			return;
		}

		openALContext = alcCreateContext(device, NULL);
		if (!openALContext) {
			std::cout << "CONTEXT NULL" << std::endl;
		}

		ALCboolean contextMadeCurrent = false;
		if (!alcMakeContextCurrent(openALContext)) {
			std::cout << "CONTEXT CURRENT FAIL" << std::endl;
		}

		openAudioFiles();
	}

	void terminate() override {
		ALCboolean contextMadeCurrent = false;
		alcMakeContextCurrent(nullptr);

		alcDestroyContext(openALContext);

		ALCboolean closed;
		alcCloseDevice(device);
	}

	void openAudioFiles() override
	{
		for (auto iterator = audioFileNames.begin(); iterator != audioFileNames.end(); iterator++)
		{
			std::string audioFileLocation = folderPath + *iterator + ".ogg";
			audioFiles.push_back(std::make_shared<OpenALAudioFile>(audioFileLocation));
		}
	}
};

#endif