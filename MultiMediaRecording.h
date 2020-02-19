extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>

#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

void FFMPEG_ERROR(int ret) {
	if (ret < 0) {
		
		char buffer[50];
		size_t bufsize = 50;
		av_strerror(ret, buffer, bufsize);
		std::cout << "\n Thrown error message: ";
		for (int i = 0; i < bufsize; i++) std::cout << buffer[i];
	}
}

//mirror image horizontally https://emvlo.wordpress.com/2016/03/10/sws_scale/
void MirrorFrameHorizontallyJ420(AVFrame* pFrame) {
	for (int i = 0; i < 4; i++) {
		if (i)
			pFrame->data[i] += pFrame->linesize[i] * ((pFrame->height >> 1) - 1);
		else
			pFrame->data[i] += pFrame->linesize[i] * (pFrame->height - 1);

		pFrame->linesize[i] = -pFrame->linesize[i];
	}
}

float fromShortToFloat(short input) {
	
	float ret = ( (float) input) / (float) 32768;
	
	if (ret > 1)
		ret = 1;
	if (ret < -1)
		ret = -1;

	return ret;
}


class Stream {

protected:
	AVStream* stream = nullptr;
	AVCodec* codec;

	AVFrame* frame = nullptr;
	AVFrame* tempFrame = nullptr;

public:
	void FreeFrames();
	bool have = false;
	AVPacket* packet = nullptr;
	int64_t nextPts = 0;
	AVCodecContext* codecContext = nullptr;
};

void Stream::FreeFrames() {
	avcodec_free_context(&codecContext);
	av_frame_free(&frame);
	av_frame_free(&tempFrame);
	av_packet_unref(packet);
}

class VideoParameters {
private:
	int width;
	int height;
	AVCodecID codecID;
	int bitrate;
	AVPixelFormat pixelFormat;
	int streamFrameRate;

public:
	VideoParameters(int, int, AVCodecID, int, AVPixelFormat, int);

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	AVCodecID GetCodecID() { return codecID; }
	int GetBitrate() { return bitrate; }
	AVPixelFormat GetPixelFormat() { return pixelFormat; }
	int GetStreamFrameRate() { return streamFrameRate; }
	void FreeFrames();

};

VideoParameters::VideoParameters(int widthIn, int heightIn, AVCodecID codecIDin, int bitrateIn, AVPixelFormat pixelFormatIn, int streamFrameRateIn) {
	width = widthIn;
	height = heightIn;
	codecID = codecIDin;
	bitrate = bitrateIn;
	pixelFormat = pixelFormatIn;
	streamFrameRate = streamFrameRateIn;
}

class AudioParameters {
private:
	int sampleRate;
	AVCodecID codecID;
	int bitrate;
	uint64_t channelLayout;
	AVSampleFormat aVSampleFormat;

public:
	AudioParameters(int, AVCodecID, int, uint64_t, AVSampleFormat);

	int GetSampleRate() { return sampleRate; }
	AVCodecID GetCodecID() { return codecID; }
	int GetBitrate() { return bitrate; }
	uint64_t GetChannelLayout() { return channelLayout; }
	AVSampleFormat GetAVSampleFormat() { return aVSampleFormat; } 
};

AudioParameters::AudioParameters(int sampleRateIn, AVCodecID codecIDIn, int bitrateIn, uint64_t channelLayoutIn, AVSampleFormat aVsampleFormatIn) {
	
	sampleRate = sampleRateIn;
	codecID = codecIDIn;
	bitrate = bitrateIn;
	channelLayout = channelLayoutIn;
	aVSampleFormat = aVsampleFormatIn;
}

class AudioStream : public Stream {
private:
	struct SwrContext *swr_ctx;

	//left channel
	float *l;
	//right channel
	float *r;

	int audioSampleCounter = 0;

public:
	AudioStream(AudioParameters*, AudioParameters*, AVFormatContext*);
	void EncodeFrame(float);
};

AudioStream::AudioStream(AudioParameters* outputAudioParameters, AudioParameters* inputAudioParameters, AVFormatContext* aVFormatContext) {
	aVFormatContext->oformat->audio_codec = outputAudioParameters->GetCodecID();

	codec = avcodec_find_encoder(outputAudioParameters->GetCodecID());

	if (!(codec)) {
		//fprintf(stderr, "Could not find encoder for '%s'\n", avcodec_get_name(codec_id));
		exit(1);
	}

	stream = avformat_new_stream(aVFormatContext, NULL);

	if (!stream) {
		//fprintf(stderr, "Could not allocate stream\n");
		exit(1);
	}

	stream->id = aVFormatContext->nb_streams - 1;

	codecContext = avcodec_alloc_context3(codec);

	if (!codecContext) {
		//fprintf(stderr, "Could not alloc an encoding context\n");
		exit(1);
	}

	stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	//video.audioStream->codecpar->codec_id = AV_CODEC_ID_AC3;
	//video.audioStream->codecpar->format = AV_SAMPLE_FMT_FLTP;

	codecContext->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
	codecContext->bit_rate = outputAudioParameters->GetBitrate();	
	stream->codecpar->sample_rate = outputAudioParameters->GetBitrate();

	//is this really needed?
	if (codec->supported_samplerates) {
		codecContext->sample_rate = codec->supported_samplerates[0];
		for (int i = 0; codec->supported_samplerates[i]; i++) {
			std::cout << "\n supported samplerates: " + codec->supported_samplerates[i];
			if (codec->supported_samplerates[i] == 44100)
				codecContext->sample_rate = 44100;
		}
	}

	std::cout << "\n samplerate: " << codecContext->sample_rate;

	//saját gányolás
	codecContext->sample_rate = outputAudioParameters->GetSampleRate();

	codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);
	codecContext->channel_layout = outputAudioParameters->GetChannelLayout();

	//is this really needed?
	if (codec->channel_layouts) {
		codecContext->channel_layout = codec->channel_layouts[0];
		for (int i = 0; codec->channel_layouts[i]; i++) {
			if (codec->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
				codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
		}
	}
	codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);

	//saját gányolás
	codecContext->time_base = { 1, outputAudioParameters->GetSampleRate() };

	stream->time_base = { 1, codecContext->sample_rate };

	//std::cout << "\n samplerate: " << video.AudioCodecContext->sample_rate;
	//std::cout << "\n audiostream time base den: " << video.audioStream->time_base.den;
	//std::cout << "\n audiostream time base num: " << video.audioStream->time_base.num;


	if (aVFormatContext->oformat->flags & AVFMT_GLOBALHEADER) codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	int nb_samples;
	int ret = avcodec_open2(codecContext, codec, NULL);

	if (ret < 0) {
		//fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
		exit(1);
	}


	if (codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples = 10000;
	else
		nb_samples = codecContext->frame_size;

	frame = av_frame_alloc();

	if (!frame) {
		fprintf(stderr, "Error allocating an audio frame\n");
		exit(1);
	}

	frame->format = codecContext->sample_fmt;
	frame->channel_layout = codecContext->channel_layout;
	frame->sample_rate = codecContext->sample_rate;
	frame->nb_samples = nb_samples;

	av_frame_make_writable(frame);

	ret = av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		fprintf(stderr, "Error allocating an audio buffer\n");
		exit(1);
	}

	ret = avcodec_parameters_from_context(stream->codecpar, codecContext);
	if (ret < 0) {
		fprintf(stderr, "Could not copy the stream parameters\n");
		exit(1);
	}

	swr_ctx = swr_alloc();
	if (!swr_ctx) {
		fprintf(stderr, "Could not allocate resampler context\n");
		exit(1);
	}

	// set options
	av_opt_set_int(swr_ctx, "in_channel_count", codecContext->channels, 0);
	av_opt_set_int(swr_ctx, "in_sample_rate", inputAudioParameters->GetSampleRate(), 0);
	av_opt_set_int(swr_ctx, "in_channel_layout", inputAudioParameters->GetChannelLayout(), 0);
	av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", inputAudioParameters->GetAVSampleFormat(), 0);
	av_opt_set_int(swr_ctx, "out_channel_count", codecContext->channels, 0);
	av_opt_set_int(swr_ctx, "out_sample_rate", outputAudioParameters->GetSampleRate(), 0);
	av_opt_set_int(swr_ctx, "out_channel_layout", outputAudioParameters->GetChannelLayout(), 0);
	av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", outputAudioParameters->GetAVSampleFormat(), 0);

	// initialize the resampling context 
	if ((ret = swr_init(swr_ctx)) < 0) {
		fprintf(stderr, "Failed to initialize the resampling context\n");
		exit(1);
	}

	packet = av_packet_alloc();
	av_init_packet(packet);
}

void AudioStream::EncodeFrame(float out) {
	if (audioSampleCounter == 0) {
		l = (float*)frame->extended_data[0];
		r = (float*)frame->extended_data[1];
	}

	if (audioSampleCounter % 2 == 0) *(l)++ = fromShortToFloat(out);
	else *(r)++ = fromShortToFloat(out);

	if (++audioSampleCounter == frame->nb_samples*codecContext->channels) {

		audioSampleCounter = 0;

		frame->pts = nextPts;

		nextPts += frame->nb_samples;
		//std::cout <<"\n audio pts: " << video.AudioFrame->pts;

		frame->pts = av_rescale_q(frame->pts, codecContext->time_base, stream->time_base);
		frame->pkt_dts = frame->pts;

		int ret = avcodec_send_frame(codecContext, frame);
		if (ret < 0) {
			std::cout << "\n send_frame: " << ret;
			exit(1);
		}

		ret = avcodec_receive_packet(codecContext, packet);
		if (ret < 0) {
			std::cout << "\n receive packet: " << ret;
			exit(1);
		}

		packet->stream_index = stream->index;
		packet->dts = frame->pkt_dts;
		packet->pts = frame->pts;

		have = true;
	}
}

class VideoStream : public Stream {
private:
	
	struct SwsContext *sws_ctx;
	bool firstVideoFrameInitialised = false;
	VideoParameters* input;
	VideoParameters* output;
	float recordStartTime;

public:
	unsigned char *buffer;
	VideoStream(VideoParameters*, VideoParameters*, AVFormatContext*);
	void FirstFrameDeInitialize();
	void EncodeFrame(float);
	void SetRecordStartTime(float);
};

void VideoStream::SetRecordStartTime(float time) {
	recordStartTime = time;
}

void VideoStream::FirstFrameDeInitialize() {
	firstVideoFrameInitialised = false;
}

VideoStream::VideoStream(VideoParameters* outputIn, VideoParameters* inputIn, AVFormatContext* aVFormatContext) {
	
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
		FFMPEG_ERROR(ret);
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

void VideoStream::EncodeFrame(float time) {
	av_frame_make_writable(frame);
	av_frame_make_writable(tempFrame);

	av_image_fill_arrays(tempFrame->data, tempFrame->linesize, buffer, input->GetPixelFormat(), input->GetWidth(), input->GetHeight(), 1);
	MirrorFrameHorizontallyJ420(tempFrame);
	sws_scale(sws_ctx, tempFrame->data, tempFrame->linesize, 0, input->GetHeight(), frame->data, frame->linesize);

	//video.Frame->pts = video.video_next_pts++;
	//video.Frame->pts = av_rescale_q(video.Frame->pts, video.VideoCodecContext->time_base, video.videoStream->time_base);

	frame->pkt_dts = (int)(1000 * (time - recordStartTime));
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
		recordStartTime = glfwGetTime();
		frame->pkt_dts = 0;
		frame->pts = 1;
	}

	packet->dts = frame->pkt_dts;
	packet->pts = frame->pts;

	packet->stream_index = stream->index;
}


class MultiMedia {
	
private:
	std::string fileName;

	AudioParameters* audioParametersIn;
	AudioParameters* audioParametersOut;

	VideoParameters* videoParametersIn;
	VideoParameters* videoParametersOut;

	bool initialized = false;

public:
	AudioStream* audio;
	VideoStream* video;

	AVFormatContext* formatContext = nullptr;

	MultiMedia(VideoParameters*, VideoParameters*, AudioParameters*, AudioParameters*);
	void Initialize(std::string);
	void InitializeAudioOnly(std::string);
	void InitializeVideoOnly(std::string);
	void CloseVideo();
	bool Initialized() { return initialized; }
};

MultiMedia::MultiMedia (VideoParameters* outputVideoParameters, VideoParameters* inputVideoParameters, AudioParameters* outputAudioParameters, AudioParameters* inputAudioParameters) {
	
	audioParametersIn = inputAudioParameters;
	audioParametersOut = outputAudioParameters;

	videoParametersIn = inputVideoParameters;
	videoParametersOut = outputVideoParameters;
}

void MultiMedia::Initialize(std::string fileNameIn) {
	fileName = fileNameIn;
	char* filename = new char[fileNameIn.length() + 1];
	fileNameIn.copy(filename, fileNameIn.length(), 0);
	filename[fileNameIn.length()] = '\0';

	//const char *name = sname.c_str();

	std::cout << "\n Recording started: " << fileNameIn << " ..." << std::endl;

	int ret = avformat_alloc_output_context2(&(formatContext), av_guess_format("matroska", filename, NULL), NULL, NULL);

	audio = new AudioStream(audioParametersOut, audioParametersIn, formatContext);
	video = new VideoStream(videoParametersOut, videoParametersIn, formatContext);

	ret = avio_open(&formatContext->pb, filename, AVIO_FLAG_WRITE);

	if (ret < 0) {
		FFMPEG_ERROR(ret);
		exit(1);
	}

	ret = avformat_write_header(formatContext, NULL);

	if (ret < 0) {
		FFMPEG_ERROR(ret);
		exit(1);
	}

	initialized = true;
}

void MultiMedia::CloseVideo() {
	initialized = false;
	video->FirstFrameDeInitialize();

	av_write_trailer(formatContext);

	//setting correct duration of streams
	//video.audioStream->duration = av_rescale_q(video.audio_next_pts, video.AudioCodecContext->time_base, video.audioStream->time_base);;
	//video.videoStream->duration = av_rescale_q(video.video_next_pts, video.VideoCodecContext->time_base, video.videoStream->time_base);;

	audio->FreeFrames();
	video->FreeFrames();

	delete[] video->buffer;
	delete video;
	delete audio;
	
	avio_closep(&formatContext->pb);
	avformat_free_context(formatContext);

	std::cout << "\n Recording ended: " + fileName;
}