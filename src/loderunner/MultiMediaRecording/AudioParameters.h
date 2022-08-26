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

	int getSampleRate() { return sampleRate; }
	AVCodecID getCodecID() { return codecID; }
	int getBitrate() { return bitrate; }
	uint64_t getChannelLayout() { return channelLayout; }
	AVSampleFormat getAVSampleFormat() { return aVSampleFormat; }
};

#endif // !AUDIOPARAMETERS_H
