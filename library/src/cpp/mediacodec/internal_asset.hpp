#pragma once
#include <android/asset_manager_jni.h>
#include <memory>
#include <string_view>
#include <string>
extern "C" {
#include <libavformat/avformat.h>
}

class internal_asset {
    private:
        std::shared_ptr<AAsset> m_asset;
    public:
        internal_asset(std::string_view path, AAsset *asset);
        AVIOContext *generate_avio();

        const std::string m_path;
};