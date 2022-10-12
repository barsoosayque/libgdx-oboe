#pragma once

#include <memory>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/error.h>
}

using format_context_ptr = std::shared_ptr<AVFormatContext>;
using avio_context_ptr = std::shared_ptr<AVIOContext>;
using codec_context_ptr = std::shared_ptr<AVCodecContext>;
using swr_context_ptr = std::shared_ptr<SwrContext>;
using frame_ptr = std::shared_ptr<AVFrame>;
using packet_ptr = std::shared_ptr<AVPacket>;

/// Convert AVERROR error to proper string message
inline std::string av_err_str(int error) {
    return std::string(av_err2str(error));
}

/// Convert AVCodecParameters* to codec's name
inline std::string codec_params_to_name(const AVCodecParameters *codec_params) {
    return std::string{ avcodec_profile_name(codec_params->codec_id, codec_params->profile) };
}

/// Make RAII AVFormatContext
inline format_context_ptr make_format_context(AVFormatContext *ctx) {
    return format_context_ptr(ctx, [](AVFormatContext *ctx) {
        avformat_close_input(&ctx);
        avformat_free_context(ctx);
    });
}

/// Make RAII AVIOContext
inline avio_context_ptr make_avio_context(AVIOContext *ctx) {
    return avio_context_ptr(ctx, [](AVIOContext *ctx) {
        av_free(ctx->buffer);
        av_free(ctx);
    });
}

/// Make RAII AVCodecContext
inline codec_context_ptr make_codec_context(AVCodecContext *ctx) {
    return codec_context_ptr(ctx, [](AVCodecContext *ctx) { avcodec_free_context(&ctx); });
}

/// Make RAII SwrContext
inline swr_context_ptr make_swr_context() {
    return swr_context_ptr(swr_alloc(), [](SwrContext *ctx) { swr_free(&ctx); });
}

/// Make RAII AVFrame
inline frame_ptr make_frame() {
    return frame_ptr(av_frame_alloc(), [](AVFrame *frame) { av_frame_free(&frame); });;
}

/// Make RAII AVPacket
inline packet_ptr make_packet() {
    return packet_ptr(av_packet_alloc(), [](AVPacket *packet) { av_packet_free(&packet); });;
}
