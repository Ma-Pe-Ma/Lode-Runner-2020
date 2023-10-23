#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <iostream>
#include <functional>

#include "Stream.h"
#include "VideoParameters.h"

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
	VideoParameters* inputVideoParameters;
	VideoParameters* outputVideoParameters;

	SwsContext* sws_ctx;
	unsigned char* buffer;
	
	bool firstVideoFrameInitialised = false;
	std::chrono::system_clock::time_point recordStartTime;

	std::function<void(unsigned char*)> readScreenBufferData;
public:
	
	VideoStream(VideoParameters*, VideoParameters*, AVFormatContext*);

	~VideoStream() {
		//setting correct duration of the stream
		//stream->duration = av_rescale_q(video.next_pts, codecContext->time_base, stream->time_base);;

		freeFrames();

		delete[] buffer;
		delete inputVideoParameters;
		delete outputVideoParameters;
	}

	void encodeFrame();

	void setScreenBufferDataReader(std::function<void(unsigned char*)> readScreenBufferData) {
		this->readScreenBufferData = readScreenBufferData;
	}
};

#endif // !VIDEOSTREAM_H