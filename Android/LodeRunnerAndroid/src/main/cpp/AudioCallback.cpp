#include "AudioCallback.h"
#include "Audio.h"
#include "Helper.h"
#include <stdlib.h>     /* srand, rand */


oboe::DataCallbackResult AudioCallback::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {
    auto *outputData = static_cast<short *>(audioData);

    int s = 0;
    for (int i = 0; i < SOUND_FILE_NR; i++)
        if (Audio::sfx[i].getPlayStatus() == AudioStatus::playing) {

            long ret = Audio::sfx[i].readNextBuffer(Audio::pcmout[s]);

            if (ret == 0)
                Audio::sfx[i].stopAndRewind();

            s++;
        }

    //if (av_compare_ts(video.next_pts, video.AudioCodecContext->time_base,STREAM_DURATION, (AVRational) { 1, 1 }) >= 0);
    //else;

    for (int i = 0; i < numFrames; i++) {
        for (int j = 0; j < 2; j++) {
            outputData[2 * i + j] = 0;
            for (int k = 0; k < s; k++) {
                outputData[2 * i + j] += (((short(Audio::pcmout[k][2 * 2 * i + 2 * j] << 8)) + Audio::pcmout[k][2 * 2 * i + 2 * j + 1])) / s;
            }
			
			outputData[2 * i + j] /= 8;

#ifdef VIDEO_RECORDING
            if (multiMedia != nullptr) {
				multiMedia->writeAudioFrame(dataOut);
			}
#endif
        }
    }

    return oboe::DataCallbackResult::Continue;
}