#include "MultiMediaRecording/AudioStream.h"
#include "MultiMedia.h"

AudioStream::AudioStream(AudioParameters* inputAudioParameters, AudioParameters* outputAudioParameters, AVFormatContext* avFormatContext) {
	this->inputAudioParameters = inputAudioParameters;
	this->outputAudioParameters = outputAudioParameters;

	codec = avcodec_find_encoder(outputAudioParameters->getCodecID());
	if (!(codec)) {
		std::cout << "Could not find encoder for " << avcodec_get_name(outputAudioParameters->getCodecID()) << std::endl;
		exit(1);
	}

	codecContext = avcodec_alloc_context3(codec);
	if (!codecContext) {
		std::cout << "Could not allocate an encoding context" << std::endl;
		exit(1);
	}

	codecContext->codec_type = AVMEDIA_TYPE_AUDIO;
	codecContext->codec_id = outputAudioParameters->getCodecID();
	codecContext->bit_rate = outputAudioParameters->getBitrate();

	const AVSampleFormat* sample_formats = nullptr;
	int num_sample_formats = 0;
	avcodec_get_supported_config(codecContext, nullptr, AV_CODEC_CONFIG_SAMPLE_FORMAT, 0, (const void**)&sample_formats, &num_sample_formats);

	if (sample_formats) {
		codecContext->sample_fmt = sample_formats[0];

		for (int i = 0; i < num_sample_formats; i++) {
			if (sample_formats[i] == outputAudioParameters->getAVSampleFormat()) {
				codecContext->sample_rate = outputAudioParameters->getAVSampleFormat();
				break;
			}
		}
	}

	const int* sample_rates = nullptr;
	int num_sample_rates = 0;
	avcodec_get_supported_config(codecContext, nullptr, AV_CODEC_CONFIG_SAMPLE_RATE, 0, (const void**) &sample_rates, &num_sample_rates);

	if (sample_rates) {
		codecContext->sample_rate = sample_rates[0];

		for (int i = 0; i < num_sample_rates; i++) {
			if (sample_rates[i] == outputAudioParameters->getSampleRate()) {
				codecContext->sample_rate = outputAudioParameters->getSampleRate();
				break;
			}
		}
	}

	const AVChannelLayout* ch_layouts = nullptr;
	int num_ch_layouts = 0;
	avcodec_get_supported_config(codecContext, nullptr, AV_CODEC_CONFIG_CHANNEL_LAYOUT, 0, (const void**) &ch_layouts, &num_ch_layouts);
	
	if (ch_layouts) {
		codecContext->ch_layout = ch_layouts[0];

		for (int i = 0; i < num_ch_layouts; i++) {
			//char buf[256];
			//av_channel_layout_describe(&ch_layouts[i], buf, sizeof(buf));

			if (ch_layouts[i].u.mask == outputAudioParameters->getChannelLayoutMask()) {
				codecContext->ch_layout = *outputAudioParameters->getChannelLayout();
				break;
			}
		}
	}
	
	int ret = avcodec_open2(codecContext, codec, NULL);

	if (ret < 0) {
		std::cout << "Could not open audio codec: "<< std::endl;
		MultiMedia::ffmpegError(ret);
		exit(1);
	}

	frame = av_frame_alloc();
	if (!frame) {
		std::cout << "Error allocating an audio frame" << std::endl;
		exit(1);
	}
	frame->format = codecContext->sample_fmt;
	frame->ch_layout = codecContext->ch_layout;
	frame->sample_rate = codecContext->sample_rate;
	frame->nb_samples = (codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE) ? 10000 : codecContext->frame_size;
	av_frame_make_writable(frame);

	ret = av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		std::cout << "Error allocating an audio buffer" << std::endl;
		exit(1);
	}

	stream = avformat_new_stream(avFormatContext, NULL);
	if (!stream) {
		std::cout << "Could not allocate stream" << std::endl;
		exit(1);
	}
	stream->time_base = { 1, codecContext->sample_rate };

	ret = avcodec_parameters_from_context(stream->codecpar, codecContext);
	if (ret < 0) {
		std::cout << "Could not copy the stream parameters" << std::endl;
		exit(1);
	}

	packet = av_packet_alloc();
	av_init_packet(packet);

	l = (float*)frame->extended_data[0];
	r = (float*)frame->extended_data[1];
}

AudioStream::~AudioStream() {
	freeAVContainers();

	delete inputAudioParameters;
	delete outputAudioParameters;
}

void AudioStream::encodeFrame(float out) {
	auto& c = audioSampleCounter % 2 == 0 ? l : r;
	*(c)++ = MultiMedia::fromShortToFloat(out);

	if (++audioSampleCounter == frame->nb_samples * codecContext->ch_layout.nb_channels) {
		audioSampleCounter = 0;

		l = (float*)frame->extended_data[0];
		r = (float*)frame->extended_data[1];

		frame->pts = av_rescale_q(nextPts, codecContext->time_base, stream->time_base);
		frame->pkt_dts = frame->pts;
		nextPts += frame->nb_samples;

		int ret = avcodec_send_frame(codecContext, frame);
		if (ret < 0) {
			std::cout << "Send frame error: " << ret << std::endl;
			exit(1);
		}

		ret = avcodec_receive_packet(codecContext, packet);
		if (ret < 0) {
			std::cout << "Receive packet error: " << ret << std::endl;;
			exit(1);
		}

		packet->stream_index = stream->index;
		packet->dts = frame->pkt_dts;
		packet->pts = frame->pts;

		have = true;
	}
}