#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include "AudioStatus.h"
#include <string>

#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>

class AudioFile {
protected:
	AudioStatus playStatus = AudioStatus::stopped;
	OggVorbis_File* sound;
public:
	AudioFile() {}
	AudioFile(std::string fileName)
	{
		openSoundFile(fileName);
		playStatus = AudioStatus::stopped;
	}
	
	virtual void playPause() {
		if (playStatus == AudioStatus::playing) {
			playStatus = AudioStatus::paused;
		}
		else {
			playStatus = AudioStatus::playing;
		}
	}

	virtual void stopAndRewind() {
		playStatus = AudioStatus::stopped;
		ov_pcm_seek(sound, 0);
	}

	virtual double lengthInSec() {

		return ov_time_total(sound, -1);
	}

	virtual AudioStatus getPlayStatus() {
		return playStatus;
	}	
	
	virtual void openSoundFile(std::string fileName) {
		sound = new OggVorbis_File;
		int success = ov_fopen(fileName.c_str(), sound);
	}

	virtual void closeSoundFile()
	{
		ov_clear(sound);
	}	

	long readNextBuffer(char* pcmChar, int size) {
		return ov_read(sound, pcmChar, size, 1, 2, 1, nullptr);
	}
};

#endif