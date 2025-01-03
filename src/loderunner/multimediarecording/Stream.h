#ifndef STREAM_H
#define STREAM_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class Stream {
protected:
	AVStream* stream = nullptr;
	const AVCodec* codec;

	AVFrame* frame = nullptr;
	AVFrame* rawFrame = nullptr;

	bool have = false;
	AVPacket* packet = nullptr;
	int64_t nextPts = 0;
	AVCodecContext* codecContext = nullptr;
public:
	void freeAVContainers() {
		avcodec_close(codecContext);
		avcodec_free_context(&codecContext);
		av_frame_free(&frame);
		av_frame_free(&rawFrame);
		av_packet_free(&packet);
	}

	friend class MultiMedia;
};

#endif // !STREAM_H

