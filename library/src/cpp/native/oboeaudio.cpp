#include "oboeaudio.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"
#include "../utility/log.hpp"
#include "../mediacodec/audio_decoder.hpp"
#include "../mediacodec/internal_asset.hpp"

OBOEAUDIO_METHOD(void, init) (JNIEnv* env, jobject self) {
    // set default audioEngine in OboeAudio class
    set_var_as(env, self, "audioEngine", new audio_engine(audio_engine::mode::async, 2));
}

inline jlong createMusic(JNIEnv* env, jobject self, std::unique_ptr<audio_decoder> &&decoder) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        music* ptr = new music(std::move(decoder), 2);
        engine->play(ptr);
        return reinterpret_cast<jlong>(ptr);
    }
    return 0;
}

inline jlong createSoundpool(JNIEnv* env, jobject self, std::unique_ptr<audio_decoder> &&decoder) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        decoder->decode();
        soundpool* ptr = new soundpool(std::move(decoder->m_buffer), 2);
        engine->play(ptr);
        return reinterpret_cast<jlong>(ptr);
    }
    return 0;
}

inline std::unique_ptr<audio_decoder> fromAsset(JNIEnv* env, jobject self, jobject asset_manager, jstring path) {
    auto decoder = std::make_unique<audio_decoder>();

    AAssetManager *native_manager = AAssetManager_fromJava(env, asset_manager);
    const char *native_path = env->GetStringUTFChars(path, 0);
    internal_asset asset(native_path, AAssetManager_open(native_manager, native_path, AASSET_MODE_RANDOM));
    env->ReleaseStringUTFChars(path, native_path);

    auto result = decoder->init(asset);
    if(result.isErr()) {
        error(result.unwrapErr().m_text);
    }

    return decoder;
}

inline std::unique_ptr<audio_decoder> fromPath(JNIEnv* env, jobject self, jstring path) {
    const char *native_path = env->GetStringUTFChars(path, 0);
    auto decoder = std::make_unique<audio_decoder>();
    auto result = decoder->init(native_path);
    if(result.isErr()) {
        error(result.unwrapErr().m_text);
    }
    env->ReleaseStringUTFChars(path, native_path);
    return decoder;
}

OBOEAUDIO_METHOD(jlong, createMusicFromAsset) (JNIEnv* env, jobject self, jobject asset_manager, jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromAsset(env, self, asset_manager, path);
    return createMusic(env, self, std::move(decoder));
}
OBOEAUDIO_METHOD(jlong, createMusicFromPath) (JNIEnv* env, jobject self, jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromPath(env, self, path);
    return createMusic(env, self, std::move(decoder));
}

OBOEAUDIO_METHOD(jlong, createSoundpoolFromAsset) (JNIEnv* env, jobject self, jobject asset_manager, jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromAsset(env, self, asset_manager, path);
    return createSoundpool(env, self, std::move(decoder));
}
OBOEAUDIO_METHOD(jlong, createSoundpoolFromPath) (JNIEnv* env, jobject self, jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromPath(env, self, path);
    return createSoundpool(env, self, std::move(decoder));
}

OBOEAUDIO_METHOD(jlong, createAudioEngine) (JNIEnv* env, jobject self, jint sample_rate, bool mono) {
    audio_engine* ptr = new audio_engine(audio_engine::mode::blocking, mono ? 1 : 2, sample_rate);
    ptr->resume();
    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(void, disposeEngine) (JNIEnv* env, jobject self) {
    delete get_var_as<audio_engine>(env, self, "audioEngine");
}

OBOEAUDIO_METHOD(void, resume) (JNIEnv* env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        engine->resume();
    }
}

OBOEAUDIO_METHOD(void, stop) (JNIEnv* env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        engine->stop();
    }
}
