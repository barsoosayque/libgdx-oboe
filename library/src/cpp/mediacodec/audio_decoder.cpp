#include <tuple>
#include <algorithm>
#include "audio_decoder.hpp"
#include "../utility/log.hpp"

extern "C" {
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

audio_decoder::audio_decoder(decoder_bundle &&bundle)
        : m_format_ctx(std::move(bundle.m_format_ctx))
        , m_codec_ctx(std::move(bundle.m_codec_ctx))
        , m_avio_ctx(std::move(bundle.m_avio_ctx))
        , m_swr_ctx(std::move(bundle.m_swr_ctx))
        , m_iframe(std::move(bundle.m_iframe))
        , m_oframe(std::move(bundle.m_oframe))
        , m_packet(std::move(bundle.m_packet)) { }

audio_decoder::buffer audio_decoder::decode(int samples) {
    while (m_use_flag.test_and_set(std::memory_order_acquire));
    int64_t delay = 0;
    int processed_samples = 0, error = 0, data_size;
    bool read_eof = false, decode_eof = false, request_more = true;
    if (samples > 0) {
        m_buffer.reserve(samples);
    }
    m_buffer.clear();

    // use cache from last decode
    if (!m_cache.empty()) {
        processed_samples = std::min(m_cache.size(), static_cast<size_t>(samples));
        auto begin = m_cache.begin(), end = std::next(m_cache.begin(), processed_samples);
        std::move(begin, end, std::back_inserter(m_buffer));
        m_cache.erase(begin, end);
        request_more = processed_samples < samples;
    }

    while (request_more && !(read_eof && decode_eof)) {
        if (!read_eof) {
            error = av_read_frame(m_format_ctx.get(), m_packet.get());
            decode_eof = false;
            if (error == AVERROR_EOF) {
                read_eof = true;
            } else if (error < 0) {
                warn("audio_decoder: Read frame error ({})", av_err_str(error));
            }

            // we were seeking, but seek wasn't precise.
            // so we need to drop some frames. sanity checks first.
            if (m_packet->pts > 0 && m_target_ts > 0 && m_packet->pts < m_target_ts) {
                auto stream = m_format_ctx->streams[m_packet->stream_index];
                auto delta =
                        static_cast<float>(m_target_ts - m_packet->pts) / stream->time_base.den;
                int64_t skip_frames =
                        delta * m_codec_ctx->time_base.den / m_codec_ctx->time_base.num;
//                debug("{} of {} (diff {}, delta {}): skip {} frames", m_packet->pts, m_target_ts, m_target_ts - m_packet->pts,delta, skip_frames);

                // AV_PKT_DATA_SKIP_SAMPLES layout:
                // u32le number of samples to skip from start of this packet
                // u32le number of samples to skip from end of this packet
                // u8    reason for start skip
                // u8    reason for end   skip (0=padding silence, 1=convergence)
                uint8_t *data = av_packet_get_side_data(m_packet.get(), AV_PKT_DATA_SKIP_SAMPLES,
                                                        nullptr);
                if (!data) {
                    data = av_packet_new_side_data(m_packet.get(), AV_PKT_DATA_SKIP_SAMPLES, 10);
                }

                // fixme: force little endian skip_frames
                *reinterpret_cast<uint32_t *>(data) = skip_frames;
                data[8] = 0;
            }

            if ((error = avcodec_send_packet(m_codec_ctx.get(),
                                           read_eof ? nullptr : m_packet.get()))) {
                warn("audio_decoder: Error sending packets ({})", av_err_str(error));
                break;
            }
        }

        while (!decode_eof) {
            error = avcodec_receive_frame(m_codec_ctx.get(), m_iframe.get());
            if (error == 0) {
                swr_config_frame(m_swr_ctx.get(), m_oframe.get(), m_iframe.get());
                do {
                    error = swr_convert_frame(m_swr_ctx.get(), m_oframe.get(),
                                            delay > 0 ? nullptr : m_iframe.get());

                    if (error < 0) {
                        warn("audio_decoder: Error converting demuxed data ({})", av_err_str(error));
                    } else {
                        if ((data_size = m_oframe->nb_samples * m_oframe->channels) == 0) {
                            // sometimes delay will return positive value, but there is nothing to be read
                            break;
                        }

                        auto begin = reinterpret_cast<int16_t *>(m_oframe->extended_data[0]),
                                end = begin + data_size;
                        std::move(begin, end, std::back_inserter(m_buffer));
                        processed_samples += data_size;
                    }

                    if (samples > 0 && processed_samples >= samples) {
                        request_more = false;
                    }
                } while ((delay = swr_get_delay(m_swr_ctx.get(), m_oframe->sample_rate)) > 0);
                delay = 0;
            } else if (error == AVERROR(EAGAIN)) {
                break;
            } else if (error == AVERROR_EOF) {
                decode_eof = true;
            } else {
                warn("audio_decoder: Error while trying to receive a frame from the decoder ({})",
                      av_err_str(error));
                break;
            }
        }

        av_packet_unref(m_packet.get());
    }

    if (samples > 0 && processed_samples > samples) {
        // cache anything past requested
        auto begin = std::next(m_buffer.begin(), samples), end = m_buffer.end();
        std::move(begin, end, std::back_inserter(m_cache));
        m_buffer.resize(samples);
    }

    if ((m_eof = read_eof & decode_eof)) {
        avcodec_flush_buffers(m_codec_ctx.get());
    }
    m_use_flag.clear(std::memory_order_release);

    return std::move(m_buffer);
}

audio_decoder::buffer audio_decoder::decode() {
    return decode(-1);
}

void audio_decoder::seek(float seconds) {
    while (m_use_flag.test_and_set(std::memory_order_acquire));

    auto stream = m_format_ctx->streams[m_packet->stream_index];
    m_target_ts = av_rescale_q(seconds * AV_TIME_BASE, AV_TIME_BASE_Q, stream->time_base);

    m_cache.clear();
    m_eof = false;
    avcodec_flush_buffers(m_codec_ctx.get());
    if (int error = av_seek_frame(m_format_ctx.get(), m_packet->stream_index, m_target_ts,
                                AVSEEK_FLAG_BACKWARD)) {
        warn("audio_decoder: Error while seeking ({})", av_err_str(error));
    }
    m_use_flag.clear(std::memory_order_release);
}

bool audio_decoder::is_eof() const {
    return m_eof;
}
