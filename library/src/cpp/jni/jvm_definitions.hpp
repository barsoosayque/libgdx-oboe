#pragma once
#include <jni.h>
#include "../utility/jvm_signature.hpp"

struct AssetFileDescriptor {
    jobject m_object;
    explicit operator jobject() const { return m_object; }
    static constexpr auto cls = "android/content/res/AssetFileDescriptor"_fstr;
};
