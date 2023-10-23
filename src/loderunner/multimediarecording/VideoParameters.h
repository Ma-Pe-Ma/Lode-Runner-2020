#ifndef VIDEOPARAMETERS_H
#define VIDEOPARAMETERS_H

extern "C" {
#include <libavcodec/avcodec.h>
}

class VideoParameters {
private:
	int width;
	int height;
	int bitrate;
	int streamFrameRate;
	AVCodecID codecID;	
	AVPixelFormat pixelFormat;
public:
	VideoParameters(int width, int height, int bitrate, int streamFrameRate, AVCodecID codecID, AVPixelFormat pixelFormat) {
		this->width = width;
		this->height = height;		
		this->bitrate = bitrate;
		this->streamFrameRate = streamFrameRate;
		this->codecID = codecID;
		this->pixelFormat = pixelFormat;
	}

	int getWidth() { return width; }
	int getHeight() { return height; }
	int getBitrate() { return bitrate; }
	int getStreamFrameRate() { return streamFrameRate; }
	AVCodecID getCodecID() { return codecID; }	
	AVPixelFormat getPixelFormat() { return pixelFormat; }	
};

#endif
