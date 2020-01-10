#include "oboeaudio.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"
#include "../utility/log.hpp"
#include "../mediacodec/audio_decoder.hpp"

OBOEAUDIO_METHOD(void, init) (JNIEnv* env, jobject self) {
    // set default audioEngine in OboeAudio class
    set_var_as(env, self, "audioEngine", new audio_engine(audio_engine::mode::async, 2));
}

OBOEAUDIO_METHOD(jlong, createMusic) (JNIEnv* env, jobject self, jstring path) {
    const char *native_path = env->GetStringUTFChars(path, 0);

    auto decoder = std::make_shared<audio_decoder>();
    auto result = decoder->init(native_path);
    if(result.isErr()) {
        error(result.unwrapErr().m_text);
    }

    auto ptr = new std::shared_ptr<music>();
    *ptr = std::make_shared<music>(decoder, 2);

    get_var_as<audio_engine>(env, self, "audioEngine")->play(*ptr);

    env->ReleaseStringUTFChars(path, native_path);
    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(jlong, createSoundpool) (JNIEnv* env, jobject self, jstring path) {
    const char *native_path = env->GetStringUTFChars(path, 0);
    auto decoder = audio_decoder();
    auto result = decoder.init(native_path);
    if(result.isErr()) {
        error(result.unwrapErr().m_text);
    }
    decoder.decode();
    auto ptr = new std::shared_ptr<soundpool>();
    *ptr = std::make_shared<soundpool>(std::move(decoder.m_buffer), 2);

    get_var_as<audio_engine>(env, self, "audioEngine")->play(*ptr);

    env->ReleaseStringUTFChars(path, native_path);
    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(jlong, createAudioEngine) (JNIEnv* env, jobject self, jint sample_rate, bool mono) {
    auto ptr = new std::shared_ptr<audio_engine>();
    *ptr = std::make_shared<audio_engine>(audio_engine::mode::blocking, mono ? 1 : 2, sample_rate);
    (*ptr)->resume();

    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(void, dispose) (JNIEnv* env, jobject self) {
    delete get_var_as<audio_engine>(env, self, "audioEngine");
}

OBOEAUDIO_METHOD(void, resume) (JNIEnv* env, jobject self) {
    get_var_as<audio_engine>(env, self, "audioEngine")->resume();
}

OBOEAUDIO_METHOD(void, stop) (JNIEnv* env, jobject self) {
    get_var_as<audio_engine>(env, self, "audioEngine")->stop();
}
