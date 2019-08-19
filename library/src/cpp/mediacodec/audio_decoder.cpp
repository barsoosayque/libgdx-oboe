#include "audio_decoder.hpp"

audio_decoder::audio_decoder(jni_context p_context, AssetFileDescriptor p_asset_fd)
    : m_context(p_context)
    , m_decoder_class(p_context, "barsoosayque/libgdxoboe/AudioDecoder")
    , m_pcm_class(p_context, "barsoosayque/libgdxoboe/AudioDecoder$Pcm")
    , m_decoder_object(p_context, m_decoder_class.construct(p_asset_fd)) { }

audio_decoder::~audio_decoder() {
    m_decoder_class.execute_method<void()>(m_decoder_object, "dispose");
}

std::vector<int16_t> as_vector(const jni_context& p_context, jvm_class& p_class, Pcm p_pcm) {
    int length = p_class.get_field<int>(p_pcm, "size");
    auto buffer = p_class.get_field<ByteBuffer>(p_pcm, "data");
    int16_t* pointer = reinterpret_cast<int16_t*>(p_context->GetDirectBufferAddress(buffer));
    std::vector<int16_t> v;
    if (length > 0) {
        v.insert(v.end(), std::make_move_iterator(pointer), std::make_move_iterator(pointer + length / 2)) ;
    }
    p_context->DeleteLocalRef(p_pcm);
    p_context->DeleteLocalRef(buffer);
    return v;
}

std::vector<int16_t> audio_decoder::decode(int p_samples) {
    auto scoped_env = m_context.acquire_thread();
    auto pcm = m_decoder_class.execute_method<Pcm(int)>(m_decoder_object, "decode", p_samples);
    return as_vector(m_context, m_pcm_class, pcm);
}

std::vector<int16_t> audio_decoder::decode() {
    auto scoped_env = m_context.acquire_thread();
    auto pcm = m_decoder_class.execute_method<Pcm()>(m_decoder_object, "decode");
    return as_vector(m_context, m_pcm_class, pcm);
}

void audio_decoder::seek(float p_seconds) {
    auto scoped_env = m_context.acquire_thread();
    m_decoder_class.execute_method<void(float)>(m_decoder_object, "seek", p_seconds);
}
