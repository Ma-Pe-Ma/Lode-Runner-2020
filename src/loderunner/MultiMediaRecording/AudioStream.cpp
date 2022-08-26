#include "MultiMediaRecording/AudioStream.h"

AudioStream::AudioStream(AudioParameters* inputAudioParameters, AudioParameters* outputAudioParameters, AVFormatContext* aVFormatContext) {
	aVFormatContext->oformat->audio_codec = outputAudioParameters->getCodecID();

	codec = avcodec_find_encoder(outputAudioParameters->getCodecID());

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
	codecContext->bit_rate = outputAudioParameters->getBitrate();
	stream->codecpar->sample_rate = outputAudioParameters->getBitrate();

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
	codecContext->sample_rate = outputAudioParameters->getSampleRate();

	codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);
	codecContext->channel_layout = outputAudioParameters->getChannelLayout();

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
	codecContext->time_base = { 1, outputAudioParameters->getSampleRate() };

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
	av_opt_set_int(swr_ctx, "in_sample_rate", inputAudioParameters->getSampleRate(), 0);
	av_opt_set_int(swr_ctx, "in_channel_layout", inputAudioParameters->getChannelLayout(), 0);
	av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", inputAudioParameters->getAVSampleFormat(), 0);
	av_opt_set_int(swr_ctx, "out_channel_count", codecContext->channels, 0);
	av_opt_set_int(swr_ctx, "out_sample_rate", outputAudioParameters->getSampleRate(), 0);
	av_opt_set_int(swr_ctx, "out_channel_layout", outputAudioParameters->getChannelLayout(), 0);
	av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", outputAudioParameters->getAVSampleFormat(), 0);

	// initialize the resampling context 
	if ((ret = swr_init(swr_ctx)) < 0) {
		fprintf(stderr, "Failed to initialize the resampling context\n");
		exit(1);
	}

	packet = av_packet_alloc();
	av_init_packet(packet);
}

void AudioStream::encodeFrame(float out) {
	if (audioSampleCounter == 0) {
		l = (float*) frame->extended_data[0];
		r = (float*) frame->extended_data[1];
	}

	if (audioSampleCounter % 2 == 0) {
		*(l)++ = MultiMediaHelper::fromShortToFloat(out);
	}
	else {
		*(r)++ = MultiMediaHelper::fromShortToFloat(out);
	}

	if (++audioSampleCounter == frame->nb_samples * codecContext->channels) {
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