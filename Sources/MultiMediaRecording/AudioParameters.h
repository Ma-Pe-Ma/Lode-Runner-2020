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
	AudioParameters(int, AVCodecID, int, uint64_t, AVSampleFormat);

	int GetSampleRate() { return sampleRate; }
	AVCodecID GetCodecID() { return codecID; }
	int GetBitrate() { return bitrate; }
	uint64_t GetChannelLayout() { return channelLayout; }
	AVSampleFormat GetAVSampleFormat() { return aVSampleFormat; }
};

#endif // !AUDIOPARAMETERS_H
