#include "MultimediaRecording/VideoStream.h"
#include "MultiMedia.h"

VideoStream::VideoStream(VideoParameters* inputVideoParameters, VideoParameters* outputVideoParameters, AVFormatContext* avFormatContext) {
	this->inputVideoParameters = inputVideoParameters;
	this->outputVideoParameters = outputVideoParameters;

	codec = avcodec_find_encoder(outputVideoParameters->getCodecID());
	if (!(codec)) {
		std::cout << "Could not find encoder for " << avcodec_get_name(outputVideoParameters->getCodecID()) << std::endl;
		exit(1);
	}

	codecContext = avcodec_alloc_context3(codec);
	if (!codecContext) {
		std::cout << "Could not allocate an encoding context" << std::endl;
		exit(1);
	}

	codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
	codecContext->codec_id = outputVideoParameters->getCodecID();
	codecContext->bit_rate = outputVideoParameters->getBitrate();
	codecContext->width = outputVideoParameters->getWidth();
	codecContext->height = outputVideoParameters->getHeight();
	codecContext->time_base = { 1, outputVideoParameters->getStreamFrameRate() };
	codecContext->gop_size = 12; // emit one intra frame every twelve frames at most 
	codecContext->pix_fmt = outputVideoParameters->getPixelFormat();
	codecContext->framerate = { outputVideoParameters->getStreamFrameRate(), 1};
	codecContext->max_b_frames = 1;
	av_opt_set(codecContext->priv_data, "preset", "slow", 0);

	int ret = avcodec_open2(codecContext, codec, NULL);
	if (ret < 0) {
		std::cout << "Could not open video codec: " << std::endl;
		MultiMedia::ffmpegError(ret);
		exit(1);
	}

	frame = av_frame_alloc();
	if (!frame) {
		exit(1);
	}
	frame->format = outputVideoParameters->getPixelFormat();
	frame->width = outputVideoParameters->getWidth();
	frame->height = outputVideoParameters->getHeight();
	av_frame_make_writable(frame);

	ret = av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		std::cout << "Error allocating a video buffer" << std::endl;
		exit(1);
	}

	rawFrame = av_frame_alloc();
	if (!rawFrame) {
		exit(1);
	}
	rawFrame->format = inputVideoParameters->getPixelFormat();
	rawFrame->width = inputVideoParameters->getWidth();
	rawFrame->height = inputVideoParameters->getHeight();
	av_frame_make_writable(rawFrame);

	ret = av_frame_get_buffer(rawFrame, 0);
	if (ret < 0) {
		std::cout << "Error allocating a raw video buffer" << std::endl;
		exit(1);
	}

	stream = avformat_new_stream(avFormatContext, NULL);
	if (!stream) {
		std::cout << "Could not allocate stream" << std::endl;
		exit(1);
	}

	ret = avcodec_parameters_from_context(stream->codecpar, codecContext);
	if (ret < 0) {
		std::cout << "Could not copy the stream parameters" << std::endl;
		exit(1);
	}
	stream->time_base = { 1, outputVideoParameters->getStreamFrameRate() };	
	stream->avg_frame_rate = { outputVideoParameters->getStreamFrameRate(), 1 };
	stream->codecpar->extradata_size = 40;
	stream->codecpar->extradata = (uint8_t*)av_malloc(stream->codecpar->extradata_size);
	
	sws_ctx = sws_getContext(inputVideoParameters->getWidth(), inputVideoParameters->getHeight(), inputVideoParameters->getPixelFormat(), outputVideoParameters->getWidth(), outputVideoParameters->getHeight(), outputVideoParameters->getPixelFormat(), SWS_BICUBIC, NULL, NULL, NULL);
	if (!sws_ctx) {
		std::cout << "Could not allocate rescaler context" << std::endl;
		exit(1);
	}

	packet = av_packet_alloc();
	av_init_packet(packet);

	buffer = new unsigned char[inputVideoParameters->getWidth() * inputVideoParameters->getHeight() * 3];
	recordStartTime = std::chrono::system_clock::now();
}

VideoStream::~VideoStream() {
	freeAVContainers();

	delete[] buffer;
	delete inputVideoParameters;
	delete outputVideoParameters;
}

void VideoStream::encodeFrame() {
	readScreenBufferData(buffer);

	av_image_fill_arrays(rawFrame->data, rawFrame->linesize, buffer, inputVideoParameters->getPixelFormat(), inputVideoParameters->getWidth(), inputVideoParameters->getHeight(), 1);
	MultiMedia::mirrorFrameHorizontallyJ420(rawFrame);
	sws_scale(sws_ctx, rawFrame->data, rawFrame->linesize, 0, inputVideoParameters->getHeight(), frame->data, frame->linesize);

	std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - recordStartTime;
	frame->pkt_dts = work_time.count();
	frame->pts = frame->pkt_dts + 1;

	int ret = avcodec_send_frame(codecContext, frame);
	if (ret < 0) {
		std::cout << "Send frame error: " << ret << std::endl;
		MultiMedia::ffmpegError(ret);
		exit(1);
	}

	ret = avcodec_receive_packet(codecContext, packet);
	if (ret < 0) {
		std::cout << "Receive packet error: " << ret << std::endl;
		MultiMedia::ffmpegError(ret);

		if (ret == -11) {
			//have = false;
			return;
		}
		else {			
			exit(1);
		}		
	}

	if (!firstVideoFrameInitialised) {
		firstVideoFrameInitialised = true;
		recordStartTime = std::chrono::system_clock::now();
		frame->pkt_dts = 0;
		frame->pts = 1;
	}

	packet->dts = frame->pkt_dts;
	packet->pts = frame->pts;
	packet->stream_index = stream->index;
	
	have = true;
}