#ifndef AUDIOPARAMETERS_H
#define AUDIOPARAMETERS_H

extern "C" {
#include <libavcodec/avcodec.h>
}

class AudioParameters {
private:
	int sampleRate;
	AVCodecID codecID;
	int bitrate;
	uint64_t channelLayout;
	AVSampleFormat aVSampleFormat;

public:
	AudioParameters(int sampleRate, AVCodecID codecID, int bitrate, uint64_t channelLayout, AVSampleFormat aVSampleFormat) {
		this->sampleRate = sampleRate;
		this->codecID = codecID;
		this->bitrate = bitrate;
		this->channelLayout = channelLayout;
		this->aVSampleFormat = aVSampleFormat;
	}

	int getSampleRate() { return sampleRate; }
	const AVCodecID getCodecID() { return codecID; }
	int getBitrate() { return bitrate; }
	uint64_t getChannelLayout() { return channelLayout; }
	AVSampleFormat getAVSampleFormat() { return aVSampleFormat; }
};

#endif // !AUDIOPARAMETERS_H
