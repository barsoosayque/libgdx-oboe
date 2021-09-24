#pragma once
#include <string_view>

#include <result.h>
#include "ffmpeg_utils.hpp"
#include "internal_asset.hpp"
#include "../utility/result_utils.hpp"

SIMPLE_RESULT(decoder_bundle)

/// Complete data set, required to make decoder work
class decoder_bundle {
public:
    /// Try to create a decoder_bundle from a file
    /// @param filename Full path to the file to decode
    /// @return Result with either a @p decoder_bundle, or delegated @p decoder_bundle_error
    static decoder_bundle_result create(std::string_view filename);

    /// Try to create a decoder_bundle from an internal_asset
    /// @param asset Valid internal_asset object
    /// @return Result with either a @p decoder_bundle, or delegated @p decoder_bundle_error
    static decoder_bundle_result create(const internal_asset &asset);

    format_context_ptr m_format_ctx;
    codec_context_ptr m_codec_ctx;
    avio_context_ptr m_avio_ctx;
    swr_context_ptr m_swr_ctx;
    frame_ptr m_iframe, m_oframe;
    packet_ptr m_packet;
private:
    decoder_bundle() = default;
};