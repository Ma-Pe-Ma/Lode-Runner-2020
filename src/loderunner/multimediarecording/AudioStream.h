#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

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

	//left and right channels
	float* l;
	float* r;

	int audioSampleCounter = 0;
public:
	AudioStream(AudioParameters*, AudioParameters*, AVFormatContext*);
	~AudioStream();
	void encodeFrame(float);
};

#endif // !AUDIOSTREAM_H

