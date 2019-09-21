#pragma once
#include "jni.h"
#include "../jni/jvm_object.hpp"
#include "../jni/jvm_class.hpp"
#include "../jni/jvm_definitions.hpp"
#include <vector>
#include <string_view>

class audio_decoder {
    public:
        audio_decoder(AssetFileDescriptor p_asset_fd);
        audio_decoder(audio_decoder&) = delete;
        audio_decoder(audio_decoder&&) = delete;
        ~audio_decoder();

        void decode(int p_samples);
        void decode();

        void seek(float p_seconds);
        std::vector<int16_t> m_buffer;
        bool m_eof;
    private:
        void process_result(Pcm p_pcm);
        jvm_class m_decoder_class;
        jvm_class m_pcm_class;
        jvm_object<jobject> m_decoder_object;
};