#pragma once
#include "jni.h"

#define MANAUDIO_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_ru_funnydogs_manatee_manaudio_Manaudio ## _ ## method

extern "C" {

// fun Manaudio::init(AssetManager): Unit
MANAUDIO_METHOD(void, init) (JNIEnv*, jobject, jobject);

// fun Manaudio::createSoundpool(): Unit
MANAUDIO_METHOD(jlong, createSoundpool) (JNIEnv*, jobject, jstring);

// fun Manaudio::dispose(): Unit
MANAUDIO_METHOD(void, dispose) (JNIEnv*, jobject);

// fun Manaudio::resume(): Unit
MANAUDIO_METHOD(void, resume) (JNIEnv*, jobject);

// fun Manaudio::stop(): Unit
MANAUDIO_METHOD(void, stop) (JNIEnv*, jobject);

}
