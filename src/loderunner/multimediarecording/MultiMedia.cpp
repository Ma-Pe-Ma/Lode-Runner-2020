#include "MultiMediaRecording/MultiMedia.h"

std::tuple<unsigned int, unsigned int> MultiMedia::determineOutput(unsigned int inputX, unsigned int inputY, unsigned int wantedOutputX, unsigned int wantedOutputY) {
	unsigned int outputX, outputY;

	if (wantedOutputY > inputY) {
		outputY = inputY;
		outputX = inputX;
	}
	else {
		outputY = wantedOutputY;
		outputX = (unsigned int)((1.0f * inputX / inputY) * wantedOutputY);
	}

	outputY -= outputY % 2;
	outputX -= outputX % 2;

	return { outputX, outputY };
}

void MultiMedia::ffmpegError(int ret) {
	if (ret < 0) {
		char buffer[50];
		size_t bufsize = 50;
		av_strerror(ret, buffer, bufsize);
		buffer[bufsize] = '\0';
		std::cout << "\n Thrown error message: " << buffer << std::endl;
	}
}

//mirror image horizontally https://emvlo.wordpress.com/2016/03/10/sws_scale/
void MultiMedia::mirrorFrameHorizontallyJ420(AVFrame* pFrame) {
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

float MultiMedia::fromShortToFloat(short input) {
	float ret = ((float)input) / (float)32768;	
	return std::clamp(ret, -1.0f, 1.0f);
}

MultiMedia::MultiMedia(std::string fileName, AudioParameters* audioParametersIn, AudioParameters* audioParametersOut, VideoParameters* videoParametersIn, VideoParameters* videoParametersOut, std::function<void(unsigned char*)> readScreenBufferData) {
	this->fileName = fileName;

	char* rawFileName = new char[this->fileName.length() + 1];
	this->fileName.copy(rawFileName, this->fileName.length(), 0);
	rawFileName[this->fileName.length()] = '\0';

	std::cout << "\n Recording started: " << this-> fileName << " ..." << std::endl;
	int ret = avformat_alloc_output_context2(&formatContext, av_guess_format("matroska", rawFileName, NULL), NULL, NULL);

	if (audioParametersOut == nullptr) {
		audioStream = nullptr;
	}
	else {
		audioStream = new AudioStream(audioParametersIn, audioParametersOut, formatContext);
	}

	if (videoParametersOut == nullptr) {
		videoStream = nullptr;
	}
	else {
		videoStream = new VideoStream(videoParametersIn, videoParametersOut, formatContext);
		videoStream->setScreenBufferDataReader(readScreenBufferData);
	}

	ret = avio_open(&formatContext->pb, rawFileName, AVIO_FLAG_WRITE);

	if (ret < 0) {
		MultiMedia::ffmpegError(ret);
		exit(1);
	}

	ret = avformat_write_header(formatContext, NULL);

	if (ret < 0) {
		MultiMedia::ffmpegError(ret);
		exit(1);
	}
}

MultiMedia::~MultiMedia() {
	av_write_trailer(formatContext);

	delete videoStream;
	delete audioStream;

	avio_closep(&formatContext->pb);
	avformat_free_context(formatContext);
	//delete formatContext;

	std::cout << "Recording ended: " + this->fileName << std::endl;
}

void MultiMedia::writeVideoFrame() {
	videoStream->encodeFrame();

	while (audioStream->have || videoStream->have) {
		if (videoStream->have && !audioStream->have) {
			av_interleaved_write_frame(formatContext, videoStream->packet);
			videoStream->have = false;
		}
		else if (audioStream->have && videoStream->have && av_compare_ts(videoStream->nextPts, videoStream->codecContext->time_base, audioStream->nextPts, audioStream->codecContext->time_base) <= 0) {
			av_interleaved_write_frame(formatContext, videoStream->packet);
			videoStream->have = false;
		}
		else {
			av_interleaved_write_frame(formatContext, audioStream->packet);
			audioStream->have = false;
		}
	}	
}

void MultiMedia::writeAudioFrame(short out) {
	audioStream->encodeFrame(out);
}