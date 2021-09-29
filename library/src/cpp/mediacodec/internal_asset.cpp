#include "internal_asset.hpp"
#include "../utility/log.hpp"

namespace {
int asset_read(void *opaque, uint8_t *buf, int buf_size) {
    auto asset = static_cast<std::shared_ptr<AAsset> *>(opaque);
    return AAsset_read(asset->get(), buf, buf_size);
}

int64_t asset_seek(void *opaque, int64_t offset, int whence) {
    auto asset = static_cast<std::shared_ptr<AAsset> *>(opaque);
    switch (whence) {
        case AVSEEK_SIZE: {
            return AAsset_getLength64(asset->get());
        }
        case AVSEEK_FORCE: {
            debug("Force seek requested. Skipping...");
            return -1;
        }
        default: {
            return AAsset_seek64(asset->get(), offset, whence);
        }
    }
}
}

internal_asset_result internal_asset::create(std::string_view path, AAssetManager *manager) {
    if (!manager) {
        return make_error("Invalid AAssetManager (nullptr)");
    }

    auto *asset = AAssetManager_open(manager, path.data(), AASSET_MODE_RANDOM);
    if (!asset) {
        return make_error("Can't open an asset: {}", path);
    }

    return ok(internal_asset(path, asset));
}

internal_asset::internal_asset(std::string_view path, AAsset *asset)
        : m_path(path)
        , m_asset(asset, &AAsset_close) {}

avio_context_ptr internal_asset::generate_avio() const {
    int buf_size = 1024 * 4;
    auto *buffer = static_cast<unsigned char *>(av_malloc(buf_size));
    return make_avio_context(
            avio_alloc_context(buffer, buf_size, 0, new std::shared_ptr<AAsset>(m_asset),
                               &asset_read, nullptr, &asset_seek));
}

