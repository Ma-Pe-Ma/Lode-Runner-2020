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

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	AVCodecID GetCodecID() { return codecID; }
	int GetBitrate() { return bitrate; }
	AVPixelFormat GetPixelFormat() { return pixelFormat; }
	int GetStreamFrameRate() { return streamFrameRate; }
	void FreeFrames();

	void setWidth(int width) { this->width = width; }
	void setHeight(int height) { this->height = height;  }
};

#endif
