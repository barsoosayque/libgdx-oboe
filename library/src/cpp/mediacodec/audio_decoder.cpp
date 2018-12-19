#include "audio_decoder.hpp"

audio_decoder::audio_decoder(jni_context p_context, AssetFileDescriptor p_asset_fd)
    : m_context(p_context)
    , m_decoder_class(p_context, "barsoosayque/libgdxoboe/AudioDecoder")
    , m_decoder_object(m_decoder_class.construct(p_asset_fd)) { }

audio_decoder::~audio_decoder() {
    m_decoder_class.execute_method<void()>(m_decoder_object, "dispose");
}

std::vector<int16_t> audio_decoder::decode(int p_samples) {
    auto buffer = m_decoder_class.execute_method<ByteBuffer(int)>(m_decoder_object, "decode", p_samples);
    return buffer.as_vector(m_context);
}

std::vector<int16_t> audio_decoder::decode() {
    auto buffer = m_decoder_class.execute_method<ByteBuffer()>(m_decoder_object, "decode");
    return buffer.as_vector(m_context);
}

void audio_decoder::seek(float p_seconds) {
    m_decoder_class.execute_method<void(float)>(m_decoder_object, "seek", p_seconds);
}
