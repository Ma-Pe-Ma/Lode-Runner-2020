#ifndef OPENALAUDIOFILE_H
#define OPENALAUDIOFILE_H

#include "AudioFile.h"

#include <AL/al.h>
#include <AL/alc.h>

class OpenALAudioFile : public AudioFile {
	ALint state = AL_STOPPED;
	ALuint aBuffer;
	ALuint source = -1;

	double lengthInSecs = 0;

public:
	OpenALAudioFile(std::string filePath) {
		openSoundFile(filePath);
	}

	void playPause() override {
		alGetSourcei(source, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING) {
			alSourcePause(source);
		}
		else {
			alSourcePlay(source);
		}
	}	

	void stopAndRewind() override {
		alSourceStop(source);
	}

	double lengthInSec() override {
		return lengthInSecs;
	}

	AudioStatus getPlayStatus() override {
		alGetSourcei(source, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING) {
			return AudioStatus::playing;
		}

		if (state == AL_PAUSED) {
			return AudioStatus::paused;
		}

		if (state == AL_STOPPED || state == AL_INITIAL) {
			return AudioStatus::stopped;
		}

		return AudioStatus::stopped;
	}

	void openSoundFile(std::string fileName) override
	{
		AudioFile::openSoundFile(fileName);

		vorbis_info* soundInfo = ov_info(sound, -1);
		ogg_int64_t total = ov_pcm_total(sound, -1) * soundInfo->channels * 2;

		ALenum format = (soundInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		ALsizei freq = soundInfo->rate;

		short* pcmout = new short[total];

		for (size_t size = 0, offset = 0, sel = 0;
			(size = ov_read(sound, (char*)pcmout + offset, 4096, 0, 2, 1, (int*)&sel)) != 0;
			offset += size) {
			if (size < 0) {
				std::cout << "Corrupted file: " << fileName;
			}
		}

		alGenBuffers(1, &aBuffer);
		alBufferData(aBuffer, format, pcmout, total, freq);
		delete[] pcmout;

		alGenSources(1, &source);
		alSourcei(source, AL_BUFFER, aBuffer);

		alSourcef(source, AL_GAIN, 0.05f);

		lengthInSecs = ov_time_total(sound, -1);

		ov_clear(sound);
	}

	void closeSoundFile() override 
	{
		alDeleteSources(1, &source);
		alDeleteBuffers(1, &aBuffer);
	}
};

#endif