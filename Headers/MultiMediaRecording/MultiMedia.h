#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include <string>
#include "RecordingState.h"

#include "AudioParameters.h"
#include "VideoParameters.h"

#include "AudioStream.h"
#include "VideoStream.h"

#include <iostream>

class MultiMedia {

private:
	unsigned int* viewPortX;
	unsigned int* viewPortY;
	unsigned int* viewPortWidth;
	unsigned int* viewPortHeight;

	std::string fileName;

	unsigned int videoOutputWidthWanted = 0;
	unsigned int videoOutputHeightWanted = 0;

	AudioParameters* audioParametersIn;
	AudioParameters* audioParametersOut = nullptr;

	VideoParameters* videoParametersIn;
	VideoParameters* videoParametersOut = nullptr;

	AudioStream* audio;
	VideoStream* video;

	bool initialized = false;

	RecordingState recordingState = uninitialized;
	AVFormatContext* formatContext = nullptr;
	std::string(*generateName)();


public:
	MultiMedia(AudioParameters* audioParaMetersIn, AudioParameters* audioParaMetersOut, VideoParameters* videoParaMetersIn, VideoParameters* videoParaMetersOut);
	
	void setGLViewPortReferences(unsigned int*, unsigned int*, unsigned int*, unsigned int*);
	
	void Initialize();
	void CloseVideo();

	void writeVideoFrame();
	void writeAudioFrame(short);
	void recordAndControl(bool impulse);
	void windowResized();

	void setGenerateName(std::string(*generateName)());
	void setVideoOutputSizeWanted(unsigned int, unsigned int);
};

#endif // !MULTIMEDIA_H