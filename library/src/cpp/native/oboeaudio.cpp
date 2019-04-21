#include "oboeaudio.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"
#include "../mediacodec/audio_decoder.hpp"

OBOEAUDIO_METHOD(void, init) (JNIEnv* env, jobject self) {
    // set default audioEngine in OboeAudio class
    set_var_as(env, self, "audioEngine", new audio_engine(2));
}

OBOEAUDIO_METHOD(jlong, createMusic) (JNIEnv* env, jobject self, jobject fd) {
    auto decoder = std::make_shared<audio_decoder>(jni_context(env), AssetFileDescriptor { fd });
    auto ptr = new std::shared_ptr<music>();
    *ptr = std::make_shared<music>(decoder, 2);

    get_var_as<audio_engine>(env, self, "audioEngine")->play(*ptr);

    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(jlong, createSoundpool) (JNIEnv* env, jobject self, jobject fd) {
    auto decoder = audio_decoder(jni_context(env), AssetFileDescriptor { fd });
    auto pcm = decoder.decode();
    auto ptr = new std::shared_ptr<soundpool>();
    *ptr = std::make_shared<soundpool>(std::move(pcm), 2);

    get_var_as<audio_engine>(env, self, "audioEngine")->play(*ptr);

    return reinterpret_cast<jlong>(ptr);
}

OBOEAUDIO_METHOD(jlong, createAudioDevice) (JNIEnv* env, jobject self, jint sample_rate, bool mono) {
    auto ptr = new std::shared_ptr<audio_engine>();
    *ptr = std::make_shared<audio_engine>(mono ? 1 : 2, sample_rate);

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
