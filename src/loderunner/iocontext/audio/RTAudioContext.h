#ifndef RTAUDIO_H
#define RTAUDIO_H

#include "AudioContext.h"
#include "AudioFile.h"

#include <RtAudio.h>

class RTAudioContext : public AudioContext {
	char** pcmout;
	RtAudio dac = RtAudio(SOUND_API);
	RtAudio::StreamParameters parameters;
	unsigned int bufferFrames = FRAMES_PER_BUFFER;
public:
	void initialize() override {
		if (dac.getDeviceCount() < 1) {
			std::cout << "\nNo audio devices found!\n";
			exit(0);
		}
		
		parameters.deviceId = dac.getDefaultOutputDevice();
		parameters.nChannels = 2;
		parameters.firstChannel = 0;

		pcmout = new char* [audioFileNames.size()];
		for (int i = 0; i < audioFileNames.size(); i++)
		{
			pcmout[i] = new char[FRAMES_PER_BUFFER * CHANNEL_COUNT * 2];
		}

		openAudioFiles();

		try {
			dac.openStream(&parameters, NULL, RTAUDIO_SINT16, SAMPLE_RATE, &bufferFrames,
				[](void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData) -> int {
					RTAudioContext* self = static_cast<RTAudioContext*>(userData);
					short* out = (short*) outputBuffer;

					if (status) {
						std::cout << "Stream underflow detected!" << std::endl;
					}

					int s = 0;

					for (auto iterator = self->audioFiles.begin(); iterator != self->audioFiles.end(); iterator++)
					{
						std::shared_ptr<AudioFile> audioFile = *iterator;

						if (audioFile->getPlayStatus() == AudioStatus::playing)
						{
							long ret = audioFile->readNextBuffer(self->pcmout[s], FRAMES_PER_BUFFER * CHANNEL_COUNT * 2);

							if (ret == 0) {
								audioFile->stopAndRewind();
							}

							s++;
						}
					}

					//if (av_compare_ts(video.next_pts, video.AudioCodecContext->time_base,STREAM_DURATION, (AVRational) { 1, 1 }) >= 0);
					//else;

					for (int i = 0; i < nBufferFrames; i++) {
						for (int j = 0; j < CHANNEL_COUNT; j++) {
							short dataOut = 0;
							for (int k = 0; k < s; k++) {
								dataOut += (((short(self->pcmout[k][2 * CHANNEL_COUNT * i + 2 * j] << 8)) + self->pcmout[k][2 * CHANNEL_COUNT * i + 2 * j + 1])) / s;
							}

							dataOut /= 8;

#ifdef VIDEO_RECORDING
							if (self->multiMedia != nullptr) {
								self->multiMedia->writeAudioFrame(dataOut);
							}
#endif
							*out++ = dataOut;
						}
					}

					return 0;
				}, (void*) this);

			dac.startStream();
		}
		catch (RtAudioError& e) {
			e.printMessage();
			exit(0);
		}
	}

	void terminate() override {
		dac.closeStream();

		for (int i = 0; i < audioFileNames.size(); i++)
		{
			delete[] pcmout[i];
		}

		delete[] pcmout;
	}
};

#endif