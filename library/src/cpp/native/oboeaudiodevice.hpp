#pragma once

#include "jni.h"

#define OBOEAUDIODEVICE_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_barsoosayque_libgdxoboe_OboeAudioDevice_ ## method

extern "C" {

OBOEAUDIODEVICE_METHOD(jint, getLatency)(JNIEnv *, jobject);
OBOEAUDIODEVICE_METHOD(void, setVolume)(JNIEnv *, jobject, jfloat);
OBOEAUDIODEVICE_METHOD(void, writeSamples___3SII)(JNIEnv *, jobject, jshortArray, jint, jint);
OBOEAUDIODEVICE_METHOD(void, writeSamples___3FII)(JNIEnv *, jobject, jfloatArray, jint, jint);
OBOEAUDIODEVICE_METHOD(void, dispose)(JNIEnv *, jobject);
OBOEAUDIODEVICE_METHOD(bool, isMono)(JNIEnv *, jobject);
OBOEAUDIODEVICE_METHOD(void, pause)(JNIEnv *, jobject);
OBOEAUDIODEVICE_METHOD(void, resume)(JNIEnv *, jobject);

}
