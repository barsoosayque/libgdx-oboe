#include "oboeaudio.hpp"

#include <memory>
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"
#include "../utility/log.hpp"
#include "../utility/exception.hpp"
#include "../utility/jni_utils.hpp"
#include "../mediacodec/audio_decoder.hpp"
#include "../mediacodec/decoder_bundle.hpp"
#include "../mediacodec/internal_asset.hpp"
#include "../jni/jvm_signature.hpp"

OBOEAUDIO_METHOD(void, init)(JNIEnv *env, jobject self) {
    // set default audioEngine in OboeAudio class
    auto *default_engine = new audio_engine(audio_engine::mode::async, 2);
    default_engine->resume();
    set_var_as(env, self, "audioEngine", default_engine);
}

inline jlong createMusic(JNIEnv *env, jobject self, std::unique_ptr<audio_decoder> &&decoder) {
    if (!decoder)
        return 0;

    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        auto ptr = new std::shared_ptr<music>();
        *ptr = std::make_shared<music>(std::move(decoder), 2);
        engine->play(*ptr);
        return reinterpret_cast<jlong>(ptr);
    }
    return 0;
}

inline jlong createSoundpool(JNIEnv *env, jobject self, std::unique_ptr<audio_decoder> &&decoder) {
    if (!decoder)
        return 0;

    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        auto buffer = decoder->decode();
        auto ptr = new std::shared_ptr<soundpool>();
        *ptr = std::make_shared<soundpool>(buffer, 2);
        engine->play(*ptr);
        return reinterpret_cast<jlong>(ptr);
    }
    return 0;
}

inline std::unique_ptr<audio_decoder> fromAsset(JNIEnv *env, jobject self, jobject asset_manager,
                                                jstring path) {
    AAssetManager *native_manager = AAssetManager_fromJava(env, asset_manager);
    std::string native_path = jni_utf8_string(env, path);

    return internal_asset::create(native_path, native_manager)
            .and_then([](internal_asset &&asset) { return decoder_bundle::create(asset); })
            .map([](decoder_bundle &&bundle) {
                return std::make_unique<audio_decoder>(std::move(bundle));
            })
            .unwrap_or_else([](simple_error &&error) {
                throw_exception(error.m_text);
                return nullptr;
            });
}

inline std::unique_ptr<audio_decoder> fromPath(JNIEnv *env, jobject self, jstring path) {
    std::string native_path = jni_utf8_string(env, path);

    return decoder_bundle::create(native_path)
            .map([](decoder_bundle &&bundle) {
                return std::make_unique<audio_decoder>(std::move(bundle));
            })
            .unwrap_or_else([](simple_error &&error) {
                throw_exception(error.m_text);
                return nullptr;
            });
}

OBOEAUDIO_METHOD(jlong, createMusicFromAsset)(JNIEnv *env, jobject self, jobject asset_manager,
                                              jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromAsset(env, self, asset_manager, path);
    return createMusic(env, self, std::move(decoder));
}

OBOEAUDIO_METHOD(jlong, createMusicFromPath)(JNIEnv *env, jobject self, jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromPath(env, self, path);
    return createMusic(env, self, std::move(decoder));
}

OBOEAUDIO_METHOD(jlong, createSoundpoolFromAsset)(JNIEnv *env, jobject self, jobject asset_manager,
                                                  jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromAsset(env, self, asset_manager, path);
    return createSoundpool(env, self, std::move(decoder));
}

OBOEAUDIO_METHOD(jlong, createSoundpoolFromPath)(JNIEnv *env, jobject self, jstring path) {
    std::unique_ptr<audio_decoder> decoder = fromPath(env, self, path);
    return createSoundpool(env, self, std::move(decoder));
}

OBOEAUDIO_METHOD(jlong, createAudioEngine)(JNIEnv *env, jobject self, jint sample_rate,
                                           jboolean mono) {
    auto *ptr = new audio_engine(audio_engine::mode::blocking, mono? 1 : 2, sample_rate);
    ptr->resume();
    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(void, disposeEngine)(JNIEnv *env, jobject self) {
    delete_var<audio_engine>(env, self, "audioEngine");
}

OBOEAUDIO_METHOD(void, resume)(JNIEnv *env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        engine->resume();
    }
}

OBOEAUDIO_METHOD(void, pause)(JNIEnv *env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        engine->stop();
    }
}
