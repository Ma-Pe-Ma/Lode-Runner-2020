#ifndef AUDIO_H
#define AUDIO_H

#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>

#include "Enums/AudioStatus.h"

#ifdef ANDROID_VERSION
#include "../AudioCallback.h"
#include "../Helper.h"
#else
#include <RtAudio/RtAudio.h>
#endif

#ifdef VIDEO_RECORDING
#include "MultiMediaRecording/MultiMedia.h"
#endif

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
    static void CloseAudioFiles();
	OggVorbis_File* sound;
	double LengthInSec();
	void PlayPause();
	void StopAndRewind();
	char* PCM;
	void OpenSoundFile(const char*);
    void CloseSoundFile();
	AudioStatus GetPlayStatus();

	long ReadNextBuffer(char(&pcmChar)[FRAMES_PER_BUFFER * CHANNEL_COUNT * 2]);

	Audio(int, const char*);
	Audio(const char*);
	Audio() {}

#ifdef ANDROID_VERSION
	static void InitializeAudioStream(AudioCallback*);
	static oboe::ManagedStream managedStream;
	static AudioCallback* audioCallback;
    static void OnWindowClose();
    static void OnWindowResume();
#else
	static int RtAudioVorbis(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);
#endif

#ifdef VIDEO_RECORDING
	static MultiMedia* multiMedia;
#endif

	static char pcmout[SOUND_FILE_NR][FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];
	static const char soundNames[SOUND_FILE_NR][25];
};
#endif // !AUDIO_H