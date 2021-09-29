#pragma once

#include <vector>
#include <string_view>
#include <atomic>

#include "internal_asset.hpp"
#include "ffmpeg_utils.hpp"
#include "decoder_bundle.hpp"

/// Main class to perform an asset decoding.
/// Supports full/partial decoding and seeking.
class audio_decoder {
public:
    using buffer = std::vector<int16_t>;

    /// Creates an audio_decoder from a decoder_bundle
    /// @note: use one of the @p decoder_bundle::create methods
    audio_decoder(decoder_bundle &&);
    audio_decoder(audio_decoder &) = delete;
    audio_decoder(audio_decoder &&) = delete;

    /// Partial decoding
    /// @param samples Number of samples to decode
    /// @return Decoding result
    buffer decode(int samples);
    /// Full decoding
    /// @result Decoding result
    buffer decode();

    /// Perform seeking inside an asset
    /// @param seconds Absolute time to seek to (amount of time from the start)
    void seek(float seconds);

    /// @return Is decoder at end of file ?
    bool is_eof() const;

private:
    std::vector<int16_t> m_buffer;
    std::vector<int16_t> m_cache;

    bool m_eof = false;

    std::atomic_flag m_use_flag = false;
    int64_t m_target_ts = -1;

    format_context_ptr m_format_ctx;
    codec_context_ptr m_codec_ctx;
    avio_context_ptr m_avio_ctx;
    swr_context_ptr m_swr_ctx;
    frame_ptr m_iframe, m_oframe;
    packet_ptr m_packet;
};