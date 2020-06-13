#ifndef STREAM_H
#define STREAM_H

extern "C" {
#include <libavformat/avformat.h>
}

class Stream {

protected:
	AVStream* stream = nullptr;
	AVCodec* codec;

	AVFrame* frame = nullptr;
	AVFrame* tempFrame = nullptr;

public:
	void FreeFrames();
	bool have = false;
	AVPacket* packet = nullptr;
	int64_t nextPts = 0;
	AVCodecContext* codecContext = nullptr;
};

#endif // !STREAM_H

