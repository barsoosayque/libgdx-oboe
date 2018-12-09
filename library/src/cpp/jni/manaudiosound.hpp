#pragma once
#include "jni.h"

#define MANAUDIO_SOUND_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_ru_funnydogs_manatee_manaudio_ManaudioSound ## _ ## method

extern "C" {

MANAUDIO_SOUND_METHOD(void, pause__) (JNIEnv*, jobject);
MANAUDIO_SOUND_METHOD(void, pause__J) (JNIEnv*, jobject, jlong);
MANAUDIO_SOUND_METHOD(jlong, play__) (JNIEnv*, jobject);
MANAUDIO_SOUND_METHOD(jlong, play__F) (JNIEnv*, jobject, jfloat);
MANAUDIO_SOUND_METHOD(void, stop__) (JNIEnv*, jobject);
MANAUDIO_SOUND_METHOD(void, stop__J) (JNIEnv*, jobject, jlong);
MANAUDIO_SOUND_METHOD(void, resume__) (JNIEnv*, jobject);
MANAUDIO_SOUND_METHOD(void, resume__J) (JNIEnv*, jobject, jlong);
MANAUDIO_SOUND_METHOD(void, setVolume) (JNIEnv*, jobject, jlong, jfloat);
MANAUDIO_SOUND_METHOD(void, dispose) (JNIEnv*, jobject);

}
