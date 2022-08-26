#include "MultiMediaRecording/MultiMedia.h"

MultiMedia::MultiMedia(AudioParameters* audioParametersIn, AudioParameters* audioParametersOut, VideoParameters* videoParametersIn, VideoParameters* videoParametersOut) {
	this->audioParametersIn = audioParametersIn;
	this->audioParametersOut = audioParametersOut;
	this->videoParametersIn = videoParametersIn;
	this->videoParametersOut = videoParametersOut;
}

void MultiMedia::setGenerateName(std::string (*generateName)()) {
	this->generateName = generateName;
}

void MultiMedia::setVideoOutputSizeWanted(unsigned int videoOutputWidthWanted, unsigned int videoOutputHeightWanted) {
	this->videoOutputHeightWanted = videoOutputHeightWanted;
}

void MultiMedia::setGLViewPortReferences(unsigned int* viewPortX, unsigned int* viewPortY, unsigned int* viewPortWidth, unsigned int* viewPortHeight) {
	this->viewPortX = viewPortX;
	this->viewPortY = viewPortY;
	this->viewPortWidth = viewPortWidth;
	this->viewPortHeight = viewPortHeight;
}

void MultiMedia::initialize() {
	std::string fileNameString = generateName();

	char* filename = new char[fileNameString.length() + 1];
	fileNameString.copy(filename, fileNameString.length(), 0);
	filename[fileNameString.length()] = '\0';
	//const char *name = sname.c_str();

	std::cout << "\n Recording started: " << fileNameString << " ..." << std::endl;

	int ret = avformat_alloc_output_context2(&(formatContext), av_guess_format("matroska", filename, NULL), NULL, NULL);	

	if (audioParametersOut == nullptr) {
		audio = nullptr;
	}
	else {
		audio = new AudioStream(audioParametersIn, audioParametersOut, formatContext);
	}

	if (videoParametersOut == nullptr) {
		video = nullptr;
	}
	else {
		if (videoOutputHeightWanted > 0) {
			unsigned int videoOutputHeight;
			unsigned int videoOutputWidth;
			if (videoOutputWidthWanted > 0) {
				videoOutputHeight = videoOutputHeightWanted;
				videoOutputWidth = videoOutputWidthWanted;
			}
			else {
				MultiMediaHelper::determineOutput(*viewPortWidth, *viewPortHeight, videoOutputWidth, videoOutputHeight, videoOutputHeightWanted);
			}

			videoParametersOut->setWidth(videoOutputWidth);
			videoParametersOut->setHeight(videoOutputHeight);
		}

		video = new VideoStream(videoParametersIn, videoParametersOut, formatContext);
		video->setGLViewPortReferences(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
	}

	ret = avio_open(&formatContext->pb, filename, AVIO_FLAG_WRITE);

	if (ret < 0) {
		MultiMediaHelper::ffmpegError(ret);
		exit(1);
	}

	ret = avformat_write_header(formatContext, NULL);

	if (ret < 0) {
		MultiMediaHelper::ffmpegError(ret);
		exit(1);
	}

	initialized = true;
	video->setRecordStartTime();
}

void MultiMedia::closeVideo() {
	initialized = false;
	video->firstFrameDeInitialize();

	av_write_trailer(formatContext);

	//setting correct duration of streams
	//video.audioStream->duration = av_rescale_q(video.audio_next_pts, video.AudioCodecContext->time_base, video.audioStream->time_base);;
	//video.videoStream->duration = av_rescale_q(video.video_next_pts, video.VideoCodecContext->time_base, video.videoStream->time_base);;

	audio->freeFrames();
	video->freeFrames();

	delete[] video->buffer;
	delete video;
	delete audio;

	/*delete videoParametersIn;
	delete videoParametersOut;

	delete audioParametersIn;
	delete audioParametersOut;*/

	avio_closep(&formatContext->pb);
	avformat_free_context(formatContext);

	std::cout << "\n Recording ended: " + fileName;
}

void MultiMedia::writeVideoFrame() {
	if (initialized) {
		video->encodeFrame();

		while (audio->have || video->have) {
			if (video->have && (!audio->have)) {
				av_interleaved_write_frame(formatContext, video->packet);
				video->have = false;
			}
			else if (audio->have && video->have && av_compare_ts(video->nextPts, video->codecContext->time_base, audio->nextPts, audio->codecContext->time_base) <= 0) {
				av_interleaved_write_frame(formatContext, video->packet);
				video->have = false;
			}
			else {
				av_interleaved_write_frame(formatContext, audio->packet);
				audio->have = false;
			}
		}
	}
}

void MultiMedia::writeAudioFrame(short out) {
	if (recordingState == recording && initialized) {
		audio->encodeFrame(out);
	}
}

void MultiMedia::recordAndControl(bool impulse) {
	if (recordingState == uninitialized) {

		if (impulse) {
			initialize();
			recordingState = recording;
		}
	}
	else if (recordingState == recording) {
		if (impulse) {
			recordingState = closing;
		}
		else {
			writeVideoFrame();
		}
	}

	if (recordingState == closing) {
		closeVideo();
		recordingState = uninitialized;
	}
}

void MultiMedia::windowResized() {
	if (recordingState == recording) {
		recordingState = closing;
	}
}