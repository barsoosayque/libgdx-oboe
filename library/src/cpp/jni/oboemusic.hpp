#pragma once
#include "jni.h"

#define OBOEMUSIC_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_barsoosayque_libgdxoboe_OboeMusic ## _ ## method

extern "C" {

OBOEMUSIC_METHOD(void, pause__) (JNIEnv*, jobject);
OBOEMUSIC_METHOD(void, pause__J) (JNIEnv*, jobject, jlong);
OBOEMUSIC_METHOD(jlong, play__) (JNIEnv*, jobject);
OBOEMUSIC_METHOD(jlong, play__F) (JNIEnv*, jobject, jfloat);
OBOEMUSIC_METHOD(void, stop__) (JNIEnv*, jobject);
OBOEMUSIC_METHOD(void, stop__J) (JNIEnv*, jobject, jlong);
OBOEMUSIC_METHOD(void, resume__) (JNIEnv*, jobject);
OBOEMUSIC_METHOD(void, resume__J) (JNIEnv*, jobject, jlong);
OBOEMUSIC_METHOD(void, setVolume) (JNIEnv*, jobject, jlong, jfloat);
OBOEMUSIC_METHOD(void, dispose) (JNIEnv*, jobject);
OBOEMUSIC_METHOD(jlong, loop__) (JNIEnv*, jobject);
OBOEMUSIC_METHOD(jlong, loop__F) (JNIEnv*, jobject, jfloat);
OBOEMUSIC_METHOD(void, setLooping) (JNIEnv*, jobject, jlong, bool);

}
