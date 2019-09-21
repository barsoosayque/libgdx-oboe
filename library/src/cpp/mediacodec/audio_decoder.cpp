#include "audio_decoder.hpp"

audio_decoder::audio_decoder(AssetFileDescriptor p_asset_fd)
    : m_decoder_class("barsoosayque/libgdxoboe/AudioDecoder")
    , m_pcm_class("barsoosayque/libgdxoboe/AudioDecoder$Pcm")
    , m_decoder_object(m_decoder_class.construct(p_asset_fd)) { }

audio_decoder::~audio_decoder() {
    m_decoder_class.execute_method<void()>(m_decoder_object, "dispose");
}

template<class context>
audio_decoder::result as_result(context& p_context, jvm_class& p_class, Pcm p_pcm) {
    int length = p_class.get_field<int>(p_pcm, "size");
    bool isLast = p_class.get_field<bool>(p_pcm, "isLast");
    auto buffer = p_class.get_field<ByteBuffer>(p_pcm, "data");
    int16_t* p = reinterpret_cast<int16_t*>(p_context->GetDirectBufferAddress(buffer));
    return {
        length > 0 ? std::vector(std::make_move_iterator(p), std::make_move_iterator(p + length / 2)) : std::vector<int16_t>(),
        isLast
    };
}

audio_decoder::result audio_decoder::decode(int p_samples) {
    auto context = jni_context::acquire_thread();
    auto pcm = m_decoder_class.execute_method<Pcm(int)>(m_decoder_object, "decode", p_samples);
    return as_result(context, m_pcm_class, pcm);
}

audio_decoder::result audio_decoder::decode() {
    auto context = jni_context::acquire_thread();
    auto pcm = m_decoder_class.execute_method<Pcm()>(m_decoder_object, "decode");
    return as_result(context, m_pcm_class, pcm);
}

void audio_decoder::seek(float p_seconds) {
    m_decoder_class.execute_method<void(float)>(m_decoder_object, "seek", p_seconds);
}
