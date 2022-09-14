#include "Audio.h"

Audio Audio::sfx[SOUND_FILE_NR];

char Audio::pcmout[SOUND_FILE_NR][FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];

#ifndef ANDROID_VERSION
std::string assetFolder = "Assets/";
#else
std::string assetFolder = "";
#endif

const std::string Audio::soundNames[SOUND_FILE_NR] = { assetFolder + "SFX/gold.ogg",  assetFolder + "SFX/dig.ogg", assetFolder + "SFX/dig_prev.ogg",
													   assetFolder + "SFX/death.ogg", assetFolder + "SFX/everygold.ogg", assetFolder + "SFX/mainmenu.ogg",
													   assetFolder + "SFX/gameover.ogg", assetFolder + "SFX/gameplay.ogg", assetFolder + "SFX/intro.ogg",
													   assetFolder + "SFX/step1.ogg", assetFolder + "SFX/step2.ogg", assetFolder + "SFX/ladder1.ogg",
													   assetFolder + "SFX/ladder2.ogg", assetFolder + "SFX/outro.ogg", assetFolder + "SFX/pause.ogg",
													   assetFolder + "SFX/pole1.ogg", assetFolder + "SFX/pole2.ogg", assetFolder + "SFX/fall.ogg"
};

#ifdef VIDEO_RECORDING
MultiMedia* Audio::multiMedia;
#endif

void Audio::playPause() {
#ifdef __EMSCRIPTEN__
	alGetSourcei(source, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING) {
		alSourcePause(source);
	}
	else {
		alSourcePlay(source);
	}

#else 
	if (playStatus == AudioStatus::playing) {
		playStatus = AudioStatus::paused;
	}
	else {
		playStatus = AudioStatus::playing;
	}
#endif
}

void Audio::stopAndRewind() {
	
#ifdef __EMSCRIPTEN__
	alSourceStop(source);
#else
	playStatus = AudioStatus::stopped;
	ov_pcm_seek(sound, 0);
#endif
}

AudioStatus Audio::getPlayStatus() {
#ifdef __EMSCRIPTEN__
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

#else
	return playStatus;
#endif // __EMSCRIPTEN__
}

Audio::Audio(const char* fileName) {
	openSoundFile(fileName);
	playStatus = AudioStatus::stopped;
}

double Audio::lengthInSec() {
#ifdef __EMSCRIPTEN__
	return lengthInSecs;
#else
	return ov_time_total(sound, -1);
#endif
}

long Audio::readNextBuffer(char(&pcmChar)[FRAMES_PER_BUFFER * CHANNEL_COUNT * 2]) {
	return ov_read(sound, pcmChar, sizeof(pcmChar), 1, 2, 1, nullptr);
}

void Audio::openSoundFile(const char* fileName) {
	sound = new OggVorbis_File;
	ov_fopen(fileName, sound);

#ifdef __EMSCRIPTEN__
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
#endif
}

void Audio::closeSoundFile() {
#ifdef __EMSCRIPTEN__
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &aBuffer);
#else
	ov_clear(sound);
#endif
}

void Audio::openAudioFiles(const std::string soundNames[SOUND_FILE_NR]) {
	for (int i = 0; i < SOUND_FILE_NR; i++) {
		sfx[i] = Audio(soundNames[i].c_str());
	}		
}

void Audio::closeAudioFiles() {
	for (int i = 0; i < SOUND_FILE_NR; i++) {
		sfx[i].closeSoundFile();
	}
#ifdef ANDROID_VERSION
	managedStream->requestStop();
	managedStream->close();
	delete audioCallback;
#endif

#ifdef __EMSCRIPTEN__
	ALCboolean contextMadeCurrent = false;
	alcMakeContextCurrent(nullptr);

	alcDestroyContext(openALContext);

	ALCboolean closed;
	alcCloseDevice(device);
#endif 
}

#ifdef __EMSCRIPTEN__
ALCdevice* Audio::device;
ALCcontext* Audio::openALContext;

void Audio::initializeOpenAL() {
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
}
#endif

#ifdef ANDROID_VERSION
oboe::ManagedStream Audio::managedStream;
AudioCallback* Audio::audioCallback;

void Audio::onWindowClose() {
	managedStream->requestPause();
}

void Audio::onWindowResume() {
	managedStream->requestStart();
}

void Audio::initializeAudioStream(AudioCallback* audioCallback1) {
	oboe::AudioStreamBuilder builder;
	builder.setDirection(oboe::Direction::Output);
	builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
	builder.setSharingMode(oboe::SharingMode::Exclusive);
	builder.setFormat(oboe::AudioFormat::I16);
	builder.setChannelCount(oboe::ChannelCount::Stereo);
	builder.setSampleRate(44100);
	builder.setFramesPerCallback(64);
	builder.setCallback(audioCallback1);
	oboe::Result result = builder.openManagedStream(managedStream);

	if (result != oboe::Result::OK) {
		Helper::log("Problem with audio stream opening!");
	}

	oboe::AudioFormat format = managedStream->getFormat();
	Helper::log("AudioStream format is_ " + std::string(oboe::convertToText(format)));
	managedStream->requestStart();
}
#else

#ifndef __EMSCRIPTEN__

int Audio::rtAudioVorbis(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData) {
	short* out = (short*)outputBuffer;
	Audio* data = (Audio*)userData;

	if (status) {
		std::cout << "Stream underflow detected!" << std::endl;
	}

	int s = 0;
	for (int i = 0; i < SOUND_FILE_NR; i++) {
		if ((data + i)->getPlayStatus() == AudioStatus::playing) {
			long ret = (data + i)->readNextBuffer(pcmout[s]);

			if (ret == 0) {
				(data + i)->stopAndRewind();
			}

			s++;
		}
	}

	//if (av_compare_ts(video.next_pts, video.AudioCodecContext->time_base,STREAM_DURATION, (AVRational) { 1, 1 }) >= 0);
	//else;

	for (int i = 0; i < nBufferFrames; i++) {
		for (int j = 0; j < CHANNEL_COUNT; j++) {
			short dataOut = 0;
			for (int k = 0; k < s; k++) {
				dataOut += (((short(pcmout[k][2 * CHANNEL_COUNT * i + 2 * j] << 8)) + pcmout[k][2 * CHANNEL_COUNT * i + 2 * j + 1])) / s;
			}

			dataOut /= 8;

#ifdef VIDEO_RECORDING
			if (multiMedia != nullptr) {
				multiMedia->writeAudioFrame(dataOut);
			}
#endif
			* out++ = dataOut;
		}
	}

	return 0;
}

#endif

#endif // ANDROID_VERSION