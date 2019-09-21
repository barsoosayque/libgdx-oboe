#include "audio_decoder.hpp"

audio_decoder::audio_decoder(AssetFileDescriptor p_asset_fd)
    : m_decoder_class("barsoosayque/libgdxoboe/AudioDecoder")
    , m_pcm_class("barsoosayque/libgdxoboe/AudioDecoder$Pcm")
    , m_decoder_object(m_decoder_class.construct(p_asset_fd)) { }

audio_decoder::~audio_decoder() {
    m_decoder_class.execute_method<void()>(m_decoder_object, "dispose");
}

void audio_decoder::process_result(Pcm p_pcm) {
    auto context = jni_context::acquire_thread();
    int length = m_pcm_class.get_field<int>(p_pcm, "size");
    auto buffer = m_pcm_class.get_field<ByteBuffer>(p_pcm, "data");
    int16_t* p = reinterpret_cast<int16_t*>(context->GetDirectBufferAddress(buffer));
    m_buffer.clear();
    m_buffer.reserve(length / 2);
    m_buffer.insert(m_buffer.begin(), std::make_move_iterator(p), std::make_move_iterator(p + length / 2));

    m_eof = m_pcm_class.get_field<bool>(p_pcm, "isLast");
}

void audio_decoder::decode(int p_samples) {
    auto context = jni_context::acquire_thread();
    auto pcm = m_decoder_class.execute_method<Pcm(int)>(m_decoder_object, "decode", p_samples);
    process_result(pcm);
}

void audio_decoder::decode() {
    auto context = jni_context::acquire_thread();
    auto pcm = m_decoder_class.execute_method<Pcm()>(m_decoder_object, "decode");
    process_result(pcm);
}

void audio_decoder::seek(float p_seconds) {
    m_decoder_class.execute_method<void(float)>(m_decoder_object, "seek", p_seconds);
}
