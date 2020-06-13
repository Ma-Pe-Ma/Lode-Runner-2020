#include "MultimediaRecording/AudioParameters.h"

AudioParameters::AudioParameters(int sampleRateIn, AVCodecID codecIDIn, int bitrateIn, uint64_t channelLayoutIn, AVSampleFormat aVsampleFormatIn) {
	sampleRate = sampleRateIn;
	codecID = codecIDIn;
	bitrate = bitrateIn;
	channelLayout = channelLayoutIn;
	aVSampleFormat = aVsampleFormatIn;
}