#include "oboeaudio.hpp"
#include "../utility/log.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"
#include "../mediacodec/audio_decoder.hpp"

OBOEAUDIO_METHOD(void, init) (JNIEnv* env, jobject self, jobject asset_manager) {
    // set audioEngine in OboeAudio class
    AAssetManager* mgr = AAssetManager_fromJava(env, asset_manager);
    set_var_as(env, self, "audioEngine", new audio_engine(2, mgr));
}

OBOEAUDIO_METHOD(jlong, createMusic) (JNIEnv* env, jobject self, jstring path) {
    const char *cpp_path = env->GetStringUTFChars(path, NULL);
    auto music = get_var_as<audio_engine>(env, self, "audioEngine")->new_music(cpp_path);
    env->ReleaseStringUTFChars(path, cpp_path);
    return reinterpret_cast<jlong>(music);
}

OBOEAUDIO_METHOD(jlong, createSoundpool) (JNIEnv* env, jobject self, jobject fd) {
    auto decoder = audio_decoder(env, AssetFileDescriptor { fd });
    auto pcm = decoder.decode();
    auto soundpool = new class soundpool(std::move(pcm), 2);

    get_var_as<audio_engine>(env, self, "audioEngine")->play(soundpool);

    return reinterpret_cast<jlong>(soundpool);
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
