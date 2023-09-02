#include "MultiMediaRecording/MultiMediaHelper.h"

void MultiMediaHelper::determineOutput(unsigned int inputX, unsigned int inputY, unsigned int& outputX, unsigned int& outputY, unsigned int wantedOutputY) {
	if (0 < wantedOutputY && wantedOutputY< inputY) {
		outputY = wantedOutputY;

		if (outputY % 2 == 1) {
			outputY--;
		}			

		outputX = (unsigned int) (((1.0f * inputX / inputY)) * outputY);

		if (outputX % 2 == 1) {
			outputX--;
		}			
	}
	else {
		outputX = inputX;
		outputY = inputY;
	}
}

void MultiMediaHelper::ffmpegError(int ret) {
	if (ret < 0) {
		char buffer[50];
		size_t bufsize = 50;
		av_strerror(ret, buffer, bufsize);

		std::cout << "\n Thrown error message: ";
		for (int i = 0; i < bufsize; i++) {
			std::cout << buffer[i];
		}			
	}
}

//mirror image horizontally https://emvlo.wordpress.com/2016/03/10/sws_scale/
void MultiMediaHelper::mirrorFrameHorizontallyJ420(AVFrame* pFrame) {
	for (int i = 0; i < 4; i++) {
		if (i) {
			pFrame->data[i] += pFrame->linesize[i] * ((pFrame->height >> 1) - 1);
		}
		else {
			pFrame->data[i] += pFrame->linesize[i] * (pFrame->height - 1);
		}			

		pFrame->linesize[i] = -pFrame->linesize[i];
	}
}

float MultiMediaHelper::fromShortToFloat(short input) {
	float ret = ((float) input) / (float) 32768;

	if (ret > 1) {
		ret = 1;
	}
		
	if (ret < -1) {
		ret = -1;
	}		

	return ret;
}