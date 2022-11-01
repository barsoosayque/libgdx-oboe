#include "oboe_audio.hpp"

#include <memory>
#include "../audio/oboe_engine.hpp"
#include "../utility/var.hpp"
#include "../utility/log.hpp"
#include "../utility/exception.hpp"
#include "../utility/jni_utils.hpp"
#include "../mediacodec/audio_decoder.hpp"
#include "../mediacodec/decoder_bundle.hpp"
#include "../mediacodec/internal_asset.hpp"
#include "../jni/jvm_signature.hpp"
#include "../audio/audio_stream.hpp"
#include "../audio/audio_player.hpp"
#include "../music/music.hpp"
#include "../sound/soundpool.hpp"

namespace {
    constexpr std::string_view k_shared_player = "sharedAudioPlayer";
    constexpr uint8_t k_channels = 2;
}

inline audio_player* get_or_create_shared_player(JNIEnv *env, jobject self) {
    if (auto player = get_var_as<audio_player>(env, self, k_shared_player)) {
        return player;
    } else {
        auto* new_player = new audio_player();
        new_player->resume();
        set_var_as(env, self, k_shared_player, new_player);
        return new_player;
    }
}

inline jlong createMusic(JNIEnv *env, jobject self, std::unique_ptr<audio_decoder> &&decoder) {
    if (!decoder)
        return 0;

    auto* player = get_or_create_shared_player(env, self);
    auto ptr = new std::shared_ptr<music>();
    *ptr = std::make_shared<music>(std::move(decoder), k_channels);
    player->play_audio(*ptr);
    return reinterpret_cast<jlong>(ptr);
}

inline jlong createSoundpool(JNIEnv *env, jobject self, std::unique_ptr<audio_decoder> &&decoder) {
    if (!decoder)
        return 0;

    auto* player = get_or_create_shared_player(env, self);
    auto buffer = decoder->decode();
    auto ptr = new std::shared_ptr<soundpool>();
    *ptr = std::make_shared<soundpool>(buffer, k_channels);
    player->play_audio(*ptr);
    return reinterpret_cast<jlong>(ptr);

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

OBOEAUDIO_METHOD(jlong, createAudioStream)(JNIEnv *env, jobject self, jint sample_rate,
                                           jboolean mono) {
    auto *ptr = new audio_stream(mono? 1 : 2, sample_rate);
    ptr->resume();
    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(jlong, createAudioRecorder)(JNIEnv *env, jobject self, jint sample_rate,
                                            jboolean mono) {
    // TODO
    return 0;
}

OBOEAUDIO_METHOD(void, disposeEngine)(JNIEnv *env, jobject self) {
    delete_var<audio_player>(env, self, k_shared_player);
}

OBOEAUDIO_METHOD(void, resume)(JNIEnv *env, jobject self) {
    auto* player = get_or_create_shared_player(env, self);
    player->resume();
}

OBOEAUDIO_METHOD(void, pause)(JNIEnv *env, jobject self) {
    auto* player = get_or_create_shared_player(env, self);
    player->stop();
}
