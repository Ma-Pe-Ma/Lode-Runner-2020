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
	static Audio sfx[];

	static void openAudioFiles(const std::string soundNames[SOUND_FILE_NR]);
    static void closeAudioFiles();
	OggVorbis_File* sound;
	double lengthInSec();
	void playPause();
	void stopAndRewind();
	char* PCM;
	void openSoundFile(const char*);
    void closeSoundFile();
	AudioStatus getPlayStatus();

	long readNextBuffer(char(&pcmChar)[FRAMES_PER_BUFFER * CHANNEL_COUNT * 2]);

	Audio(int, const char*);
	Audio(const char*);
	Audio() {}

#ifdef ANDROID_VERSION
	static void initializeAudioStream(AudioCallback*);
	static oboe::ManagedStream managedStream;
	static AudioCallback* audioCallback;
    static void onWindowClose();
    static void onWindowResume();
#else
	static int rtAudioVorbis(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);
#endif

#ifdef VIDEO_RECORDING
	static MultiMedia* multiMedia;
#endif

	static char pcmout[SOUND_FILE_NR][FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];
	static const std::string soundNames[SOUND_FILE_NR];
};
#endif // !AUDIO_H