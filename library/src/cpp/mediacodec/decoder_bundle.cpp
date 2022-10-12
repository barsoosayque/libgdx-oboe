#include <tuple>
#include "decoder_bundle.hpp"
#include "../utility/log.hpp"
#include "../utility/result_utils.hpp"

extern "C" {
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

namespace {
using format_context_result = result<format_context_ptr, simple_error>;
using codec_result = result<std::pair<codec_context_ptr, int>, simple_error>;
using swr_result = result<std::tuple<swr_context_ptr, frame_ptr, frame_ptr, packet_ptr>,
                          simple_error>;


format_context_result create_context(std::string_view filename,
                                     AVFormatContext *format_ctx) {
    if (int error = avformat_open_input(&format_ctx, filename.data(), nullptr, nullptr)) {
        return make_error("Could not open {}: {}", filename,
                          av_err_str(error));
    }
    return ok(make_format_context(format_ctx));
}

codec_result create_codec(const format_context_ptr &format_ctx) {
    // Read streams info
    if (int error = avformat_find_stream_info(format_ctx.get(), nullptr)) {
        return make_error("Could not retrieve stream info from {}: {}",
                          format_ctx->url,
                          av_err_str(error));
    }

    const AVCodec *codec = nullptr;
    const AVCodecParameters *codec_params = nullptr;
    int stream_index = -1;

    // Find first audio stream
    for (int i = 0; i < format_ctx->nb_streams; ++i) {
        codec_params = format_ctx->streams[ i ]->codecpar;
        codec = avcodec_find_decoder(codec_params->codec_id);
        if (!codec) {
            return make_error(
                    "Unsupported codec {}", codec_params_to_name(codec_params));
        }

        if (codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream_index = i;
            break;
        }
    }
    if (stream_index == -1) {
        return make_error("Could not retrieve audio stream from {}",
                          format_ctx->url);
    }

    // Try to create a context
    if (AVCodecContext *codec_ctx = avcodec_alloc_context3(codec)) {
        if (int error = avcodec_parameters_to_context(codec_ctx, codec_params)) {
            return make_error(
                    "Failed to copy params to context for codec {}: {}",
                    codec_params_to_name(codec_params), av_err_str(error));
        }

        if (int error = avcodec_open2(codec_ctx, codec, nullptr)) {
            return make_error("Failed to open codec {}: {}",
                              codec_params_to_name(codec_params),
                              av_err_str(error));
        }

        // WAV uses pcm_s16le codec and channel layout is 0.
        // more here https://stackoverflow.com/a/20049638
        if ( codec_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC )
            av_channel_layout_default( &codec_ctx->ch_layout, codec_ctx->ch_layout.nb_channels );

        return ok(std::make_pair(make_codec_context(codec_ctx), stream_index));
    } else {
        return make_error("Failed to allocate memory for Codec Context");
    }
}

swr_result create_swr(const codec_context_ptr &codec_ctx, int stream_index) {
    frame_ptr iframe = make_frame();
    av_channel_layout_copy( &iframe->ch_layout, &codec_ctx->ch_layout );
    iframe->sample_rate = codec_ctx->sample_rate;
    iframe->format = codec_ctx->sample_fmt;

    debug("audio_decoder: iframe config:\nsample_rate: {}\nformat: {}\nchannels: {}",
          iframe->sample_rate, iframe->format, iframe->ch_layout.nb_channels );

    frame_ptr oframe = make_frame();
    // TODO: remove hardcoded channels
    oframe->ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    oframe->sample_rate = 44100;
    oframe->format = AV_SAMPLE_FMT_S16;

    debug("audio_decoder: oframe config:\nsample_rate: {}\nformat: {}\nchannels: {}",
          oframe->sample_rate, oframe->format, oframe->ch_layout.nb_channels);

    swr_context_ptr ctx = make_swr_context();
    auto* raw_ctx = ctx.get();
    swr_alloc_set_opts2(&raw_ctx,
                       &oframe->ch_layout, static_cast<AVSampleFormat>(oframe->format),
                       oframe->sample_rate,
                       &iframe->ch_layout, static_cast<AVSampleFormat>(iframe->format),
                       iframe->sample_rate,
                       0, nullptr
                      );
    int error = swr_init(ctx.get());
    if (!swr_is_initialized(ctx.get())) {
        return make_error(
                "Resampler couldn't been initialized. Error: {}",
                av_err_str(error));
    }

    packet_ptr packet = make_packet();
//    av_init_packet(packet.get());
    packet->stream_index = stream_index;

    return ok(std::make_tuple(ctx, oframe, iframe, packet));
}
}

decoder_bundle_result decoder_bundle::create(std::string_view filename) {
    decoder_bundle bundle;

    bundle.m_format_ctx = TRY(create_context(fmt::format("file:{}", filename), nullptr));
    int stream_index = 0;
    std::tie(bundle.m_codec_ctx, stream_index) = TRY(create_codec(bundle.m_format_ctx));
    std::tie(bundle.m_swr_ctx, bundle.m_oframe, bundle.m_iframe, bundle.m_packet) = TRY(
            create_swr(bundle.m_codec_ctx, stream_index));

    return ok(std::move(bundle));
}

decoder_bundle_result decoder_bundle::create(const internal_asset &asset) {
    decoder_bundle bundle;

    AVFormatContext *format_ctx = avformat_alloc_context();
    bundle.m_avio_ctx = asset.generate_avio();
    format_ctx->pb = bundle.m_avio_ctx.get();
    format_ctx->flags |= AVFMT_FLAG_CUSTOM_IO | AVFMT_FLAG_NONBLOCK;

    bundle.m_format_ctx = TRY(create_context(asset.m_path, format_ctx));
    int stream_index;
    std::tie(bundle.m_codec_ctx, stream_index) = TRY(create_codec(bundle.m_format_ctx));
    std::tie(bundle.m_swr_ctx, bundle.m_oframe, bundle.m_iframe, bundle.m_packet) = TRY(
            create_swr(bundle.m_codec_ctx, stream_index));

    return ok(std::move(bundle));
}