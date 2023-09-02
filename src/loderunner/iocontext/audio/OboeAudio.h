#ifndef OBOEAUDIO_H
#define OBOEAUDIO_H

#include "Audio.h"

#ifdef ANDROID_VERSION
#include "../AudioCallback.h"
#include "../Helper.h"
#else

class OboeAudio : public Audio
{
	oboe::ManagedStream managedStream;
	AudioCallback* audioCallback;

public:
	void initialize() override
	{
		
	}

	void terminate() override
	{
		managedStream->requestStop();
		managedStream->close();
		delete audioCallback;
	}

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
};

#endif