#include "MultiMediaRecording/Stream.h"

void Stream::freeFrames() {
	avcodec_free_context(&codecContext);
	av_frame_free(&frame);
	av_frame_free(&tempFrame);
	av_packet_unref(packet);
}