#ifndef AUDIO_H
#define AUDIO_H

#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>

#include "Enums/AudioStatus.h"
#include "Variables.h"

#include <RtAudio/RtAudio.h>
#include "MultiMediaRecording/MultiMedia.h"

#define SOUND_FILE_NR 18
#define FRAMES_PER_BUFFER 64
#define CHANNEL_COUNT 2
#define SAMPLE_RATE 44100

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

	static int RtAudioVorbis(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);
	static MultiMedia* multiMedia;

	static char pcmout[SOUND_FILE_NR][FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];
	static const char soundNames[SOUND_FILE_NR][25];
};
#endif // !AUDIO_H