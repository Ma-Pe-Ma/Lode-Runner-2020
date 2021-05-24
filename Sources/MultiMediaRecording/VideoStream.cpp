#include "MultimediaRecording/VideoStream.h"

void VideoStream::setGLViewPortReferences(unsigned int* viewPortX, unsigned int* viewPortY, unsigned int* viewPortWidth, unsigned int* viewPortHeight) {
	this->viewPortX = viewPortX;
	this->viewPortY = viewPortY;
	this->viewPortWidth = viewPortWidth;
	this->viewPortHeight = viewPortHeight;
}

void VideoStream::SetRecordStartTime() {
	recordStartTime = std::chrono::system_clock::now();
}

void VideoStream::FirstFrameDeInitialize() {
	firstVideoFrameInitialised = false;
}

VideoStream::VideoStream(VideoParameters* inputIn, VideoParameters* outputIn, AVFormatContext* aVFormatContext) {
	input = inputIn;
	output = outputIn;

	//video
	codec = avcodec_find_encoder(output->GetCodecID());
	if (!(codec)) {
		//fprintf(stderr, "Could not find encoder for '%s'\n",avcodec_get_name(codec_id));
		exit(1);
	}

	stream = avformat_new_stream(aVFormatContext, NULL);

	if (!stream) {
		fprintf(stderr, "Could not allocate stream\n");
		exit(1);
	}

	stream->id = aVFormatContext->nb_streams - 1;

	codecContext = avcodec_alloc_context3(codec);

	if (!codecContext) {
		fprintf(stderr, "Could not alloc an encoding context\n");
		exit(1);
	}

	aVFormatContext->flags |= AVFMT_FLAG_GENPTS;
	aVFormatContext->oformat->flags &= AVFMT_NOFILE;

	if (aVFormatContext == NULL) {
		//cerr << "Could not allocate output context" << endl;
		//return -1;
	}

	std::cout << "width: " << output->GetWidth();
	std::cout << "height: " << output->GetHeight();

	codecContext->codec_id = output->GetCodecID();
	codecContext->bit_rate = output->GetBitrate();
	codecContext->width = output->GetWidth();
	codecContext->height = output->GetHeight();
	stream->time_base = { 1, STREAM_FRAME_RATE };
	codecContext->time_base = stream->time_base;
	codecContext->gop_size = 12; // emit one intra frame every twelve frames at most 
	codecContext->pix_fmt = output->GetPixelFormat();

	av_opt_set(codecContext->priv_data, "preset", "slow", 0);
	//video.VideoCodecContext->framerate = { 25, 1 };
	//video.videoStream->index = 0;

	int ret = avcodec_open2(codecContext, codec, NULL);
	if (ret < 0) {
		//fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
		MultiMediaHelper::FFMPEG_ERROR(ret);
		exit(1);
	}

	frame = av_frame_alloc();
	if (!frame) {
		exit(1);
	}

	frame->format = output->GetPixelFormat();
	frame->width = output->GetWidth();
	frame->height = output->GetHeight();

	//TODO: What is the second argument?
	av_frame_get_buffer(frame, 0);

	//int numBytes = av_image_get_buffer_size(outputVideoParameters.GetPixelFormat(), outputVideoParameters.GetWidth(), outputVideoParameters.GetHeight(), 1);
	//uint8_t *picture_buf = (uint8_t *)av_malloc(numBytes);
	//av_image_fill_arrays(frame->data, frame->linesize, picture_buf, outputVideoParameters.GetPixelFormat(), outputVideoParameters.GetWidth(), outputVideoParameters.GetHeight(), 1);

	tempFrame = av_frame_alloc();
	if (!tempFrame) {
		exit(1);
	}

	tempFrame->format = input->GetPixelFormat();
	tempFrame->width = input->GetWidth();
	tempFrame->height = input->GetHeight();

	//TODO: What is the second argument?
	av_frame_get_buffer(tempFrame, 0);

	buffer = new unsigned char[input->GetWidth() * input->GetHeight() * 3];
	av_image_fill_arrays(tempFrame->data, tempFrame->linesize, buffer, input->GetPixelFormat(), input->GetWidth(), input->GetHeight(), 1);

	sws_ctx = sws_getContext(input->GetWidth(), input->GetHeight(), input->GetPixelFormat(), output->GetWidth(), output->GetHeight(), output->GetPixelFormat(), SWS_BICUBIC, NULL, NULL, NULL);

	ret = avcodec_parameters_from_context(stream->codecpar, codecContext);
	if (ret < 0) {
		fprintf(stderr, "Could not copy the stream parameters\n");
		exit(1);
	}

	std::cout << "\n video.videoStream->codecpar->format: " << stream->codecpar->format;

	//video.videoStream->codecpar->codec_tag = (uint32_t )av_malloc(32);
	//video.videoStream->codecpar->format = 0;
	stream->avg_frame_rate = { output->GetStreamFrameRate(), 1 };

	//stream extradata
	stream->codecpar->extradata_size = 40;
	//video.videoStream->codecpar->extradata_size = 43;
	stream->codecpar->extradata = (uint8_t*)av_malloc(stream->codecpar->extradata_size);
	//char ED[43] = { 1, 100, 0, 41, -1, -31, 0, 25, 103, 100, 0, 41, -84, 82, 12, 1, 64, 16, -20, 4, 64, 0, 0, -6, 64, 0, 46, -32, 35, -58, 12, 99, 96, 1, 0, 7, 104, -24, -50, 10, -60, -56, 76 };
	//for (int i = 0; i < video.videoStream->codecpar->extradata_size; i++) *(video.videoStream->codecpar->extradata + i) = ED[i];

	codecContext->max_b_frames = 1;
	codecContext->codec_tag = 1;
	codecContext->codec_type = AVMEDIA_TYPE_VIDEO;

	if (aVFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
		codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	packet = av_packet_alloc();
	av_init_packet(packet);
}

void VideoStream::EncodeFrame() {
	glReadPixels(*viewPortX, *viewPortY, *viewPortWidth, *viewPortHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	av_frame_make_writable(frame);
	av_frame_make_writable(tempFrame);

	av_image_fill_arrays(tempFrame->data, tempFrame->linesize, buffer, input->GetPixelFormat(), input->GetWidth(), input->GetHeight(), 1);
	MultiMediaHelper::MirrorFrameHorizontallyJ420(tempFrame);
	sws_scale(sws_ctx, tempFrame->data, tempFrame->linesize, 0, input->GetHeight(), frame->data, frame->linesize);

	//video.Frame->pts = video.video_next_pts++;
	//video.Frame->pts = av_rescale_q(video.Frame->pts, video.VideoCodecContext->time_base, video.videoStream->time_base);

	//frame->pkt_dts = (int)(1000 * (time - recordStartTime));
	std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - recordStartTime;
	frame->pkt_dts = work_time.count();
	frame->pts = frame->pkt_dts + 1;

	//std::cout << "\n vidi pts rescaled: " << video.Frame->pts;

	int ret = avcodec_send_frame(codecContext, frame);

	if (ret < 0) {
		std::cout << "\n send_frame: " << ret;
		exit(1);
	}

	ret = avcodec_receive_packet(codecContext, packet);

	if (ret < 0 && ret != -11) {
		std::cout << "\n receive packet: " << ret;
		exit(1);
	}

	have = true;

	if (ret < 0) {
		std::cout << "\nError: invalid frame!";
		have = false;
	}

	if (!firstVideoFrameInitialised && ret == 0) {
		std::cout << "\nFirst frame initialised!";
		firstVideoFrameInitialised = true;
		recordStartTime = std::chrono::system_clock::now();
		frame->pkt_dts = 0;
		frame->pts = 1;
	}

	packet->dts = frame->pkt_dts;
	packet->pts = frame->pts;

	packet->stream_index = stream->index;
}