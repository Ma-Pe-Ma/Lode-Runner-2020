#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include <string>

#include "AudioParameters.h"
#include "VideoParameters.h"

#include "AudioStream.h"
#include "VideoStream.h"

#include <iostream>

class MultiMedia {

private:
	std::string fileName;

	AudioStream* audioStream = nullptr;
	VideoStream* videoStream = nullptr;

	AVFormatContext* formatContext = nullptr;
public:
	static std::tuple<unsigned int, unsigned int> determineOutput(unsigned int, unsigned int, unsigned int, unsigned int);
	static void ffmpegError(int);
	static float fromShortToFloat(short input);
	static void mirrorFrameHorizontallyJ420(AVFrame* pFrame);

	MultiMedia(std::string, AudioParameters*, AudioParameters*, VideoParameters*, VideoParameters*, std::function<void(unsigned char*)>);
	~MultiMedia();

	void writeVideoFrame();
	void writeAudioFrame(short);
};

#endif // !MULTIMEDIA_H