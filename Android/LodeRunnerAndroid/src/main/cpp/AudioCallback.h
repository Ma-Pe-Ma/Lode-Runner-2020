#ifndef LODERUNNER_AUDIOCALLBACK_H
#define LODERUNNER_AUDIOCALLBACK_H

#include <oboe/Oboe.h>

class AudioCallback : public oboe::AudioStreamCallback {
public:
    AudioCallback() {}
    ~AudioCallback() {}

    oboe::DataCallbackResult onAudioReady (oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames);
};

#endif //LODERUNNER_AUDIOCALLBACK_H
