#include "Audio.h"

Audio Audio::SFX[SOUND_FILE_NR];

char Audio::pcmout[SOUND_FILE_NR][FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];


const char Audio::soundNames[SOUND_FILE_NR][25] = { "SFX/gold.ogg", "SFX/dig.ogg","SFX/dig_prev.ogg",
										"SFX/death.ogg", "SFX/everygold.ogg","SFX/mainmenu.ogg",
										"SFX/gameover.ogg", "SFX/gameplay.ogg","SFX/intro.ogg",
										"SFX/step1.ogg","SFX/step2.ogg","SFX/ladder1.ogg",
										"SFX/ladder2.ogg", "SFX/outro.ogg","SFX/pause.ogg",
										"SFX/pole1.ogg","SFX/pole2.ogg", "SFX/fall.ogg"
};

MultiMedia* Audio::multiMedia;

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

int Audio::RtAudioVorbis(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData) {
	short* out = (short*)outputBuffer;
	Audio* data = (Audio*)userData;

	if (status)
		std::cout << "Stream underflow detected!" << std::endl;

	int s = 0;
	for (int i = 0; i < SOUND_FILE_NR; i++)
		if ((data + i)->GetPlayStatus() == playing) {

			long ret = (data + i)->ReadNextBuffer(pcmout[s]);

			if (ret == 0)
				(data + i)->StopAndRewind();

			s++;
		}

	//if (av_compare_ts(video.next_pts, video.AudioCodecContext->time_base,STREAM_DURATION, (AVRational) { 1, 1 }) >= 0);
	//else;

	for (int i = 0; i < nBufferFrames; i++)
		for (int j = 0; j < CHANNEL_COUNT; j++) {
			short dataOut = 0;
			for (int k = 0; k < s; k++)
				dataOut += (((short(pcmout[k][2 * CHANNEL_COUNT * i + 2 * j] << 8)) + pcmout[k][2 * CHANNEL_COUNT * i + 2 * j + 1])) / s;

#ifdef VIDEO_RECORDING
			if (multiMedia != nullptr) {
				multiMedia->writeAudioFrame(dataOut);
			}
#endif
			* out++ = dataOut;
		}

	return 0;
}