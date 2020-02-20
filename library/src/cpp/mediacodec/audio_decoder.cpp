#include <tuple>
#include <algorithm>
#include "audio_decoder.hpp"
#include "../utility/log.hpp"
extern "C" {
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

Result<format_context_ptr, decoder_error> audio_decoder::create_context(std::string_view filename,
                                                                      AVFormatContext *format_ctx) {
    if (int err = avformat_open_input(&format_ctx, filename.data(), nullptr, nullptr)) {
        return Err(decoder_error{fmt::format("Could not open {}: {}", filename, av_err_str(err))});
    }
    return Ok(make_format_context(format_ctx));
}

Result<std::pair<codec_context_ptr, int>, decoder_error> audio_decoder::create_codec(const format_context_ptr &format_ctx) {
    // Read streams info
    if (int err = avformat_find_stream_info(format_ctx.get(), nullptr)) {
        return Err(decoder_error{fmt::format("Could not retrieve stream info from {}: {}", format_ctx->url, av_err_str(err))});
    }

    AVCodec *codec;
    AVCodecParameters *codec_params;
    int stream_index = -1;

    // Find first audio stream
    for (int i = 0; i < format_ctx->nb_streams; ++i) {
        codec_params = format_ctx->streams[i]->codecpar;
        codec = avcodec_find_decoder(codec_params->codec_id);
        if (!codec) {
            return Err(decoder_error{fmt::format("Unsupported codec {}", codec_params_to_name(codec_params))});
        }

        if (codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream_index = i;
            break;
        }
    }
    if (stream_index == -1) {
        return Err(decoder_error{fmt::format("Could not retrieve audio stream from {}", format_ctx->url)});
    }

    // Try to create a context
    if (AVCodecContext *codec_ctx = avcodec_alloc_context3(codec)) {
        if (int err = avcodec_parameters_to_context(codec_ctx, codec_params)) {
            return Err(decoder_error{fmt::format("Failed to copy params to context for codec {}: {}", codec_params_to_name(codec_params), av_err_str(err))});
        }

        if (int err = avcodec_open2(codec_ctx, codec, nullptr)) {
            return Err(decoder_error{fmt::format("Failed to open codec {}: {}", codec_params_to_name(codec_params), av_err_str(err))});
        }

        return Ok(std::make_pair(make_codec_context(codec_ctx), stream_index));
    } else {
        return Err(decoder_error{"Failed to allocate memory for Codec Context"}) ;
    }
}

Result<std::tuple<swr_context_ptr, frame_ptr, frame_ptr, packet_ptr>, decoder_error> audio_decoder::create_swr(const codec_context_ptr &codec_ctx, int stream_index) {
    frame_ptr iframe = make_frame();
    iframe->channel_layout = codec_ctx->channel_layout;
    iframe->sample_rate = codec_ctx->sample_rate;
    iframe->format = codec_ctx->sample_fmt;
    iframe->channels = codec_ctx->channels;

    frame_ptr oframe = make_frame();
    // TODO: remove hardcoded channels
    oframe->channel_layout = AV_CH_LAYOUT_STEREO;
    oframe->sample_rate = 44100;
    oframe->format = AV_SAMPLE_FMT_S16;
    oframe->channels = av_get_channel_layout_nb_channels(oframe->channel_layout);

    swr_context_ptr ctx = make_swr_context();
    swr_alloc_set_opts(ctx.get(),
        oframe->channel_layout, static_cast<AVSampleFormat>(oframe->format), oframe->sample_rate,
        iframe->channel_layout, static_cast<AVSampleFormat>(iframe->format), iframe->sample_rate,
        0, nullptr
    );
    av_opt_set_int(ctx.get(), "in_channel_count",  iframe->channels, 0);
    av_opt_set_int(ctx.get(), "out_channel_count", oframe->channels, 0);
    swr_init(ctx.get());
    if (!swr_is_initialized(ctx.get())) {
        return Err(decoder_error{"Resampler couldn't been initialized."}) ;
    }

    packet_ptr packet = make_packet();
    av_init_packet(packet.get());
    packet->stream_index = stream_index;

    return Ok(std::make_tuple(ctx, oframe, iframe, packet));
}

decoder_result audio_decoder::init(internal_asset &asset) {
    AVFormatContext *format_ctx = avformat_alloc_context();
    m_avio_ctx = make_avio_context(asset.generate_avio());
    format_ctx->pb = m_avio_ctx.get();
    format_ctx->flags |= AVFMT_FLAG_CUSTOM_IO | AVFMT_FLAG_NONBLOCK;

    m_format_ctx = TRY(create_context(asset.m_path, format_ctx));
    int stream_index;
    std::tie(m_codec_ctx, stream_index) = TRY(create_codec(m_format_ctx));
    std::tie(m_swr_ctx, m_oframe, m_iframe, m_packet) = TRY(create_swr(m_codec_ctx, stream_index));

    return Ok();
}

decoder_result audio_decoder::init(std::string_view filename) {
    m_format_ctx = TRY(create_context(fmt::format("file:{}", filename), nullptr));
    int stream_index;
    std::tie(m_codec_ctx, stream_index) = TRY(create_codec(m_format_ctx));
    std::tie(m_swr_ctx, m_oframe, m_iframe, m_packet) = TRY(create_swr(m_codec_ctx, stream_index));

    return Ok();
}

void audio_decoder::decode(int samples) {
    while(m_use_flag.test_and_set(std::memory_order_acquire));
    int64_t delay = 0;
    int processed_samples = 0, err = 0, data_size;
    bool read_eof = false, decode_eof = false, request_more = true;
    if(samples > 0) {
        m_buffer.reserve(samples);
    }
    m_buffer.clear();

    // use cache from last decode
    if(!m_cache.empty()) {
        processed_samples = std::min(m_cache.size(), static_cast<size_t>(samples));
        auto begin = m_cache.begin(), end = std::next(m_cache.begin(), processed_samples);
        std::move(begin, end, std::back_inserter(m_buffer));
        m_cache.erase(begin, end);
        request_more = processed_samples < samples;
    }

    while (request_more && !(read_eof && decode_eof)) {
        if(!read_eof) {
            err = av_read_frame(m_format_ctx.get(), m_packet.get());
            decode_eof = false;
            if(err == AVERROR_EOF) {
                read_eof = true;
            } else if (err < 0) {
                error("audio_decoder: Read frame error ({})", av_err_str(err));
            }

            // we were seeking, but seek wasn't precise.
            // so we need to drop some frames. sanity checks first.
            if(m_packet->pts > 0 && m_target_ts > 0 && m_packet->pts < m_target_ts) {
                auto stream = m_format_ctx->streams[m_packet->stream_index];
                auto delta = static_cast<float>(m_target_ts - m_packet->pts) / stream->time_base.den;
                int64_t skip_frames = delta * m_codec_ctx->time_base.den / m_codec_ctx->time_base.num;
//                debug("{} of {} (diff {}, delta {}): skip {} frames", m_packet->pts, m_target_ts, m_target_ts - m_packet->pts,delta, skip_frames);

                // AV_PKT_DATA_SKIP_SAMPLES layout:
                // u32le number of samples to skip from start of this packet
                // u32le number of samples to skip from end of this packet
                // u8    reason for start skip
                // u8    reason for end   skip (0=padding silence, 1=convergence)
                uint8_t *data = av_packet_get_side_data(m_packet.get(), AV_PKT_DATA_SKIP_SAMPLES, nullptr);
                if(!data) {
                     data = av_packet_new_side_data(m_packet.get(), AV_PKT_DATA_SKIP_SAMPLES, 10);
                }

                // fixme: force little endian skip_frames
                *reinterpret_cast<uint32_t*>(data) = skip_frames;
                data[8] = 0;
            }

            if ((err = avcodec_send_packet(m_codec_ctx.get(), read_eof ? nullptr : m_packet.get()))) {
                error("audio_decoder: Error sending packets ({})", av_err_str(err));
                break;
            }
        }

        while(!decode_eof) {
            err = avcodec_receive_frame(m_codec_ctx.get(), m_iframe.get());
            if (err == 0) {
                swr_config_frame(m_swr_ctx.get(), m_oframe.get(), m_iframe.get());
                do {
                    err = swr_convert_frame(m_swr_ctx.get(), m_oframe.get(), delay > 0 ? nullptr : m_iframe.get());

                    if(err < 0) {
                        error("audio_decoder: Error converting demuxed data ({})", av_err_str(err));
                    } else {
                        if((data_size = m_oframe->nb_samples * m_oframe->channels) == 0) {
                            // sometimes delay will return positive value, but there is nothing to be read
                            break;
                        }

                        auto begin = reinterpret_cast<int16_t*>(m_oframe->extended_data[0]),
                             end = begin + data_size;
                        std::move(begin, end, std::back_inserter(m_buffer));
                        processed_samples += data_size;
                    }

                    if(samples > 0 && processed_samples >= samples) {
                        request_more = false;
                    }
                } while ((delay = swr_get_delay(m_swr_ctx.get(), m_oframe->sample_rate)) > 0);
                delay = 0;
            } else if (err == AVERROR(EAGAIN)) {
                break;
            } else if (err == AVERROR_EOF) {
                decode_eof = true;
            } else {
                error("audio_decoder: Error while trying to receive a frame from the decoder ({})", av_err_str(err));
                break;
            }
        }

        av_packet_unref(m_packet.get());
    }

    if(samples > 0 && processed_samples > samples) {
        // cache anything past requested
        auto begin = std::next(m_buffer.begin(), samples), end = m_buffer.end();
        std::move(begin, end, std::back_inserter(m_cache));
        m_buffer.resize(samples);
    }

    if((m_eof = read_eof & decode_eof)) {
        avcodec_flush_buffers(m_codec_ctx.get());
    }
    m_use_flag.clear(std::memory_order_release);
}

void audio_decoder::decode() {
    decode(-1);
}

void audio_decoder::seek(float seconds) {
    while(m_use_flag.test_and_set(std::memory_order_acquire));

    auto stream = m_format_ctx->streams[m_packet->stream_index];
    m_target_ts = av_rescale_q(seconds * AV_TIME_BASE, AV_TIME_BASE_Q, stream->time_base);

    m_cache.clear();
    m_eof = false;
    avcodec_flush_buffers(m_codec_ctx.get());
    if(int err = av_seek_frame(m_format_ctx.get(), m_packet->stream_index, m_target_ts, AVSEEK_FLAG_BACKWARD)) {
        error("audio_decoder: Error while seeking ({})", av_err_str(err));
    }
    m_use_flag.clear(std::memory_order_release);
}
