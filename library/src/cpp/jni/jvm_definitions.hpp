#pragma once
#include <jni.h>
#include "jvm_signature.hpp"
#include <vector>
#include <iterator>

struct AssetFileDescriptor {
    static constexpr auto cls = "android/content/res/AssetFileDescriptor"_fstr;
    jobject m_object;

    explicit operator jobject() const { return m_object; }
};
