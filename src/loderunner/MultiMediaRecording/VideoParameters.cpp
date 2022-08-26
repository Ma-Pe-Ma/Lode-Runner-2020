#include "MultiMediaRecording/VideoParameters.h"

VideoParameters::VideoParameters(int widthIn, int heightIn, AVCodecID codecIDin, int bitrateIn, AVPixelFormat pixelFormatIn, int streamFrameRateIn) {
	width = widthIn;
	height = heightIn;
	codecID = codecIDin;
	bitrate = bitrateIn;
	pixelFormat = pixelFormatIn;
	streamFrameRate = streamFrameRateIn;
}