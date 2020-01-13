#pragma once
#include <vector>
#include <string_view>

#include <result.h>
#include "internal_asset.hpp"
#include "ffmpeg_utils.hpp"

struct decoder_error {
    std::string m_text;
};
using decoder_result = Result<void, decoder_error>;

class audio_decoder {
    private:
        std::vector<int16_t> m_cache;

        format_context_ptr m_format_ctx;
        codec_context_ptr m_codec_ctx;
        avio_context_ptr m_avio_ctx;
        swr_context_ptr m_swr_ctx;
        frame_ptr m_iframe, m_oframe;
        packet_ptr m_packet;

        static Result<format_context_ptr, decoder_error>
        create_context(std::string_view filename, AVFormatContext *format_ctx = nullptr);

        static Result<std::pair<codec_context_ptr, int>, decoder_error>
        create_codec(const format_context_ptr &format_ctx);

        static Result<std::tuple<swr_context_ptr, frame_ptr, frame_ptr, packet_ptr>, decoder_error>
        create_swr(const codec_context_ptr& codec_ctx, int stream_index);
    public:
        audio_decoder() = default;
        audio_decoder(audio_decoder&) = delete;
        audio_decoder(audio_decoder&&) = delete;

        decoder_result init(std::string_view filename);
        decoder_result init(internal_asset &asset);
        void decode(int samples);
        void decode();
        void seek(float seconds);

        std::vector<int16_t> m_buffer;
        bool m_eof = false;
};