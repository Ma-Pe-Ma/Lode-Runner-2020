#include "Audio.h"

Audio Audio::SFX[SOUND_FILE_NR];

void Audio::PlayPause() {
	if (playStatus == playing)
		playStatus = paused;
	else
		playStatus = playing;
}

void Audio::StopAndRewind() {
	playStatus = stopped;
	ov_pcm_seek(sound, 0);
}

AudioStatus Audio::GetPlayStatus() {
	return playStatus;
}

Audio::Audio(const char* fileName) {
	OpenSoundFile(fileName);
	playStatus = stopped;
}

Audio::Audio(int indexIn, const char* fileName) {
	index = indexIn;
	OpenSoundFile(fileName);
	playStatus = stopped;
}

double Audio::LengthInSec() {
	return ov_time_total(sound, -1);
}

long Audio::ReadNextBuffer(char(&pcmChar)[FRAMES_PER_BUFFER * CHANNEL_COUNT * 2]) {
	return ov_read(sound, pcmChar, sizeof(pcmChar), 1, 2, 1, nullptr);
}

void Audio::OpenSoundFile(const char* fileName) {
	sound = new OggVorbis_File;
	ov_fopen(fileName, sound);
}

void Audio::OpenAudioFiles(const char soundNames[SOUND_FILE_NR][25]) {
	for (int i = 0; i < SOUND_FILE_NR; i++)
		SFX[i] = Audio(i, soundNames[i]);
}