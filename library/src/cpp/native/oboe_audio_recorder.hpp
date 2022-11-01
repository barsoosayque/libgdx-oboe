#pragma once

#include "jni.h"

#define OBOEAUDIORECORDER_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_barsoosayque_libgdxoboe_OboeAudioRecorder_ ## method

extern "C" {

OBOEAUDIORECORDER_METHOD(void, read) (JNIEnv * , jobject , jshortArray , jint , jint ) ;
OBOEAUDIORECORDER_METHOD(void, dispose) (JNIEnv * , jobject ) ;

}
