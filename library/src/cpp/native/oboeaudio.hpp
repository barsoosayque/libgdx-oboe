#pragma once
#include "jni.h"

#define OBOEAUDIO_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_barsoosayque_libgdxoboe_OboeAudio ## _ ## method

extern "C" {

// fun OboeAudio::init(AssetManager): Unit
OBOEAUDIO_METHOD(void, init) (JNIEnv*, jobject);

// fun OboeAudio::createSoundpoolFromPath(): NativeSoundpool
OBOEAUDIO_METHOD(jlong, createSoundpoolFromPath) (JNIEnv*, jobject, jstring);

// fun OboeAudio::createSoundpoolFromAsset(): NativeSoundpool
OBOEAUDIO_METHOD(jlong, createSoundpoolFromAsset) (JNIEnv*, jobject, jobject, jstring);

// fun OboeAudio::createMusicFromPath(): NativeMusic
OBOEAUDIO_METHOD(jlong, createMusicFromPath) (JNIEnv*, jobject, jstring);

// fun OboeAudio::createMusicFromAsset(): NativeMusic
OBOEAUDIO_METHOD(jlong, createMusicFromAsset) (JNIEnv*, jobject, jobject, jstring);

// fun OboeAudio::createAudioEngine(): NativeAudioDevice
OBOEAUDIO_METHOD(jlong, createAudioEngine) (JNIEnv*, jobject, jint, bool);

// fun OboeAudio::disposeEngine(): Unit
OBOEAUDIO_METHOD(void, disposeEngine) (JNIEnv*, jobject);

// fun OboeAudio::resume(): Unit
OBOEAUDIO_METHOD(void, resume) (JNIEnv*, jobject);

// fun OboeAudio::stop(): Unit
OBOEAUDIO_METHOD(void, stop) (JNIEnv*, jobject);

}
