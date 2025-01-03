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
	uint64_t channelLayoutMask;
	AVChannelLayout channelLayout;
	AVSampleFormat aVSampleFormat;

public:
	AudioParameters(int sampleRate, AVCodecID codecID, int bitrate, uint64_t channelLayoutMask, AVSampleFormat aVSampleFormat) {
		this->sampleRate = sampleRate;
		this->codecID = codecID;
		this->bitrate = bitrate;
		this->aVSampleFormat = aVSampleFormat;
		
		this->channelLayoutMask = channelLayoutMask;
		av_channel_layout_from_mask(&this->channelLayout, channelLayoutMask);
	}

	int getSampleRate() { return sampleRate; }
	const AVCodecID getCodecID() { return codecID; }
	int getBitrate() { return bitrate; }
	uint64_t getChannelLayoutMask() { return channelLayoutMask; }
	AVChannelLayout* getChannelLayout() { return &channelLayout; }
	AVSampleFormat getAVSampleFormat() { return aVSampleFormat; }
};

#endif // !AUDIOPARAMETERS_H
