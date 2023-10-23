#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <iostream>

#include "Stream.h"
#include "AudioParameters.h"

extern "C" {
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
}


class AudioStream : public Stream {
private:
	AudioParameters* inputAudioParameters;
	AudioParameters* outputAudioParameters;

	SwrContext* swr_ctx;

	//left channel
	float* l;
	//right channel
	float* r;

	int audioSampleCounter = 0;
public:
	AudioStream(AudioParameters*, AudioParameters*, AVFormatContext*);
	~AudioStream() {
		//setting correct duration of the stream
		//stream->duration = av_rescale_q(next_pts, codecContext->time_base, stream->time_base);
		freeFrames();

		delete inputAudioParameters;
		delete outputAudioParameters;
	}
	void encodeFrame(float);
};

#endif // !AUDIOSTREAM_H

