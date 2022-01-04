#ifndef VIDEOPARAMETERS_H
#define VIDEOPARAMETERS_H

extern "C" {
#include <libavcodec/avcodec.h>
}

class VideoParameters {
private:
	int width;
	int height;

	AVCodecID codecID;
	int bitrate;
	AVPixelFormat pixelFormat;
	int streamFrameRate;

public:
	VideoParameters(int, int, AVCodecID, int, AVPixelFormat, int);

	int getWidth() { return width; }
	int getHeight() { return height; }
	AVCodecID getCodecID() { return codecID; }
	int getBitrate() { return bitrate; }
	AVPixelFormat getPixelFormat() { return pixelFormat; }
	int getStreamFrameRate() { return streamFrameRate; }
	void freeFrames();

	void setWidth(int width) { this->width = width; }
	void setHeight(int height) { this->height = height;  }
};

#endif
