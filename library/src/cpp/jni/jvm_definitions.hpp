#pragma once
#include <jni.h>
#include "../utility/jvm_signature.hpp"
#include <vector>
#include <iterator>

struct AssetFileDescriptor {
    static constexpr auto cls = "android/content/res/AssetFileDescriptor"_fstr;
    jobject m_object;

    explicit operator jobject() const { return m_object; }
};

struct ByteBuffer {
    static constexpr auto cls = "java/nio/ByteBuffer"_fstr;
    jobject m_object;

    explicit operator jobject() const { return m_object; }
    std::vector<int16_t> as_vector(jni_context p_context) {
        int length = p_context->GetDirectBufferCapacity(m_object);
        int16_t* pointer = static_cast<int16_t*>(p_context->GetDirectBufferAddress(m_object));
        std::vector<int16_t> v;
        v.insert(v.end(), std::make_move_iterator(pointer), std::make_move_iterator(pointer + length / 2)) ;
        return v;
    }
};
