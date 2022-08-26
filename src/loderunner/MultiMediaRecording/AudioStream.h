#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <iostream>

#include "MultiMediaHelper.h"

#include "Stream.h"
#include "AudioParameters.h"

extern "C" {
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
}


class AudioStream : public Stream {
private:
	struct SwrContext* swr_ctx;

	//left channel
	float* l;
	//right channel
	float* r;

	int audioSampleCounter = 0;

public:
	AudioStream(AudioParameters*, AudioParameters*, AVFormatContext*);
	void encodeFrame(float);
};

#endif // !AUDIOSTREAM_H

