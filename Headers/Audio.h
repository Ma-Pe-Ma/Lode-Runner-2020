#ifndef AUDIO_H
#define AUDIO_H

#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>

#include "Enums/AudioStatus.h"
#include "Variables.h"

class Audio {

private:
	AudioStatus playStatus;
	int index;

public:
	static Audio SFX[];

	static void OpenAudioFiles(const char soundNames[SOUND_FILE_NR][25]);
	OggVorbis_File* sound;
	double LengthInSec();
	void PlayPause();
	void StopAndRewind();
	char* PCM;
	void OpenSoundFile(const char*);
	AudioStatus GetPlayStatus();

	long ReadNextBuffer(char(&pcmChar)[FRAMES_PER_BUFFER * CHANNEL_COUNT * 2]);

	Audio(int, const char*);
	Audio(const char*);
	Audio() {}
};

#endif // !AUDIOSTATUS_H