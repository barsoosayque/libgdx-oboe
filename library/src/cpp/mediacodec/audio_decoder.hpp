#pragma once
#include "jni.h"
#include "../jni/jvm_class.hpp"
#include "../jni/jvm_definitions.hpp"
#include <vector>
#include <string_view>

class audio_decoder {
    public:
        audio_decoder(jni_context p_context, AssetFileDescriptor p_asset_fd);
        ~audio_decoder();

        std::vector<int16_t> decode(int p_samples);
        std::vector<int16_t> decode();

        void seek(float p_seconds);
    private:
        jni_context m_context;
        jvm_class m_decoder_class;
        jobject m_decoder_object;
};