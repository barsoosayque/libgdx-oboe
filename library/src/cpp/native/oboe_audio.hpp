#pragma once

#include "jni.h"

#define OBOEAUDIO_METHOD(result, method) \
    JNIEXPORT result JNICALL \
    Java_barsoosayque_libgdxoboe_OboeAudio_ ## method

extern "C" {

// fun OboeAudio::createSoundpoolFromPath(): NativeSoundpool
OBOEAUDIO_METHOD(jlong, createSoundpoolFromPath)(JNIEnv *, jobject, jstring);

// fun OboeAudio::createSoundpoolFromAsset(): NativeSoundpool
OBOEAUDIO_METHOD(jlong, createSoundpoolFromAsset)(JNIEnv *, jobject, jobject, jstring);

// fun OboeAudio::createMusicFromPath(): NativeMusic
OBOEAUDIO_METHOD(jlong, createMusicFromPath)(JNIEnv *, jobject, jstring);

// fun OboeAudio::createMusicFromAsset(): NativeMusic
OBOEAUDIO_METHOD(jlong, createMusicFromAsset)(JNIEnv *, jobject, jobject, jstring);

// fun OboeAudio::createAudioStream(): NativeAudioStream
OBOEAUDIO_METHOD(jlong, createAudioStream)(JNIEnv *, jobject, jint, jboolean);

// fun OboeAudio::createAudioRecorder(): NativeAudioRecorder
OBOEAUDIO_METHOD(jlong, createAudioRecorder)(JNIEnv *, jobject, jint, jboolean);

// fun OboeAudio::disposeEngine(): Unit
OBOEAUDIO_METHOD(void, disposeEngine)(JNIEnv *, jobject);

// fun OboeAudio::resume(): Unit
OBOEAUDIO_METHOD(void, resume)(JNIEnv *, jobject);

// fun OboeAudio::pause(): Unit
OBOEAUDIO_METHOD(void, pause)(JNIEnv *, jobject);

}
