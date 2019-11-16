#pragma once
#include "jni.h"

#define OBOESOUND_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_barsoosayque_libgdxoboe_OboeSound ## _ ## method

extern "C" {

OBOESOUND_METHOD(void, pause__) (JNIEnv*, jobject);
OBOESOUND_METHOD(void, pause__J) (JNIEnv*, jobject, jlong);
OBOESOUND_METHOD(jlong, play__) (JNIEnv*, jobject);
OBOESOUND_METHOD(jlong, play__F) (JNIEnv*, jobject, jfloat);
OBOESOUND_METHOD(jlong, loop__) (JNIEnv*, jobject);
OBOESOUND_METHOD(jlong, loop__F) (JNIEnv*, jobject, jfloat);
OBOESOUND_METHOD(void, stop__) (JNIEnv*, jobject);
OBOESOUND_METHOD(void, stop__J) (JNIEnv*, jobject, jlong);
OBOESOUND_METHOD(void, resume__) (JNIEnv*, jobject);
OBOESOUND_METHOD(void, resume__J) (JNIEnv*, jobject, jlong);
OBOESOUND_METHOD(void, setVolume) (JNIEnv*, jobject, jlong, jfloat);
OBOESOUND_METHOD(void, setLooping) (JNIEnv*, jobject, jlong, bool);
OBOESOUND_METHOD(void, dispose) (JNIEnv*, jobject);
OBOESOUND_METHOD(void, setPitch) (JNIEnv*, jobject, jlong, jfloat);
OBOESOUND_METHOD(void, setPan) (JNIEnv*, jobject, jlong, jfloat, jfloat);
OBOESOUND_METHOD(jlong, play) (JNIEnv*, jobject, jfloat, jfloat, jfloat);
OBOESOUND_METHOD(jlong, loop) (JNIEnv*, jobject, jfloat, jfloat, jfloat);

}
