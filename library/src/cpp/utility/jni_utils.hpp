#pragma once

#include "jni.h"

inline std::string jni_utf8_string(JNIEnv *env, jstring str) {
    const char *native = env->GetStringUTFChars(str, nullptr);
    std::string result(native);
    env->ReleaseStringUTFChars(str, native);
    return result;
}