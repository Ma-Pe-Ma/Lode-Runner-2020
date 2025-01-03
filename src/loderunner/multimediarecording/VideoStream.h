#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <functional>

#include "Stream.h"
#include "VideoParameters.h"

extern "C" {
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <chrono>

class VideoStream : public Stream {
private:
	VideoParameters* inputVideoParameters;
	VideoParameters* outputVideoParameters;

	SwsContext* sws_ctx;

	bool firstVideoFrameInitialised = false;
	std::chrono::system_clock::time_point recordStartTime;

	std::function<void(unsigned char*)> readScreenBufferData;
	unsigned char* buffer;
public:
	VideoStream(VideoParameters*, VideoParameters*, AVFormatContext*);
	~VideoStream();

	void encodeFrame();

	void setScreenBufferDataReader(std::function<void(unsigned char*)> readScreenBufferData) {
		this->readScreenBufferData = readScreenBufferData;
	}
};

#endif // !VIDEOSTREAM_H