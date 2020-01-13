#include "internal_asset.hpp"
#include "../utility/log.hpp"

internal_asset::internal_asset(std::string_view path, AAsset *asset)
    : m_asset(asset, &AAsset_close)
    , m_path(path)
    {}

int asset_read(void *opaque, uint8_t *buf, int buf_size) {
    auto asset = static_cast<std::shared_ptr<AAsset>*>(opaque);
    return AAsset_read(asset->get(), buf, buf_size);
}

int64_t asset_seek(void *opaque, int64_t offset, int whence) {
    auto asset = static_cast<std::shared_ptr<AAsset>*>(opaque);
    switch(whence) {
        case AVSEEK_SIZE:
            return AAsset_getLength64(asset->get());
        case AVSEEK_FORCE:
            debug("Force seek requested. Skipping...");
            return -1;
        default:
            return AAsset_seek64(asset->get(), offset, whence);
    }
}

AVIOContext *internal_asset::generate_avio() {
    int buf_size = 1024 * 4;
    unsigned char *buffer = (unsigned char *)av_malloc(buf_size);
    return avio_alloc_context(buffer, buf_size, 0, new std::shared_ptr<AAsset>(m_asset),
                             &asset_read, NULL, &asset_seek);
}

