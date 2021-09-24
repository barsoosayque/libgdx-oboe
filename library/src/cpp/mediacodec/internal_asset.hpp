#pragma once
#include <android/asset_manager_jni.h>
#include <memory>
#include <string_view>
#include <string>
#include <result.h>

#include "ffmpeg_utils.hpp"
#include "../utility/result_utils.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

SIMPLE_RESULT(internal_asset)

/// Utility class to tie together android internal assets with avio context for reading
class internal_asset {
public:
    /// Try to wrap an android internal asset
    /// @param path Asset path
    /// @param manager Valid android asset manager
    static internal_asset_result create(std::string_view path, AAssetManager* manager);

    /// Generates a working avio context for reading/seeking an android internal asset
    /// @return RAII avio context
    avio_context_ptr generate_avio() const;

    const std::string m_path;

private:
    internal_asset(std::string_view path, AAsset *asset);

    std::shared_ptr<AAsset> m_asset;
};