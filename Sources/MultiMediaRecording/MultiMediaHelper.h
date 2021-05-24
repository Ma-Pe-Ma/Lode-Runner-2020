#ifndef MULTIMEDIAHELPER_H
#define MULTIMEDIAHELPER_H

#define STREAM_DURATION 10.0
//for correct recording: audio-nb_samples/samplefreqency > 1/streamframerate (video FPS)
#define STREAM_FRAME_RATE 60

//default pix_fmt
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P

#include <iostream>

extern "C" {
#include <libavdevice/avdevice.h>
}

class MultiMediaHelper {
public:
	static void determineOutput(unsigned int, unsigned int, unsigned int&, unsigned int&, unsigned int);
	static void FFMPEG_ERROR(int);
	static void MirrorFrameHorizontallyJ420(AVFrame* pFrame);
	static float fromShortToFloat(short input);
};

#endif // !MULTIMEDIAHELPER_H