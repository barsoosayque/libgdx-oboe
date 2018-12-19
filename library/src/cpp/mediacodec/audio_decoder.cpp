#include "audio_decoder.hpp"

audio_decoder::audio_decoder(JNIEnv* p_env, AssetFileDescriptor p_asset_fd)
    : m_decoder_class(p_env, "barsoosayque/libgdxoboe/AudioDecoder")
    , m_decoder_object(m_decoder_class.construct(p_asset_fd)) {

}

audio_decoder::~audio_decoder() {
    m_decoder_class.execute_method<void()>(m_decoder_object, "dispose");
}

void audio_decoder::seek(float p_seconds) {
    m_decoder_class.execute_method<void(float)>(m_decoder_object, "seek", p_seconds);
}
