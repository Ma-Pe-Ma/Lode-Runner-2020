#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <iostream>

#include "MultiMediaHelper.h"

#include "Stream.h"
#include "VideoParameters.h"
//#include "GLHelper.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


extern "C" {
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <chrono>

class VideoStream : public Stream {
private:
	unsigned int* viewPortX;
	unsigned int* viewPortY;
	unsigned int* viewPortWidth;
	unsigned int* viewPortHeight;

	struct SwsContext* sws_ctx;
	bool firstVideoFrameInitialised = false;
	VideoParameters* input;
	VideoParameters* output;
	std::chrono::system_clock::time_point recordStartTime;

public:
	unsigned char* buffer;
	VideoStream(VideoParameters*, VideoParameters*, AVFormatContext*);
	void firstFrameDeInitialize();
	void encodeFrame();
	void setRecordStartTime();
	void setGLViewPortReferences(unsigned int* viewPortX, unsigned int* viewPortY, unsigned int* viewPortWidth, unsigned int* viewPortHeight);
};

#endif // !VIDEOSTREAM_H