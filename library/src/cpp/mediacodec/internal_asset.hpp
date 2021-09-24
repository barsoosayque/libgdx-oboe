#pragma once
#include <android/asset_manager_jni.h>
#include <memory>
#include <string_view>
#include <string>

#include "ffmpeg_utils.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

/// Utility class to tie together android internal assets with avio context for reading
class internal_asset {
public:
    /// Wraps an android internal asset
    /// @param path Asset path
    /// @param asset Opened android asset
    internal_asset(std::string_view path, AAsset *asset);

    /// Generates a working avio context for reading/seeking an android internal asset
    /// @return RAII avio context
    avio_context_ptr generate_avio() const;

    const std::string m_path;

private:
    std::shared_ptr<AAsset> m_asset;
};