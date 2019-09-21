#pragma once
#include "jni.h"
#include "../jni/jvm_object.hpp"
#include "../jni/jvm_class.hpp"
#include "../jni/jvm_definitions.hpp"
#include <vector>
#include <string_view>

class audio_decoder {
    public:
        struct result {
            std::vector<int16_t> m_data;
            bool m_eof;
        };

        audio_decoder(AssetFileDescriptor p_asset_fd);
        audio_decoder(audio_decoder&) = delete;
        audio_decoder(audio_decoder&&) = delete;
        ~audio_decoder();

        result decode(int p_samples);
        result decode();

        void seek(float p_seconds);
    private:
        jvm_class m_decoder_class;
        jvm_class m_pcm_class;
        jvm_object<jobject> m_decoder_object;
};