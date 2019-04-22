#include "oboeaudiodevice.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"

OBOEAUDIODEVICE_METHOD(jint, getLatency) (JNIEnv* env, jobject self) {
    return shared_ptr_var<audio_engine>(env, self, "audioEngine")->payload_size();
}

OBOEAUDIODEVICE_METHOD(void, setVolume) (JNIEnv* env, jobject self, jfloat volume) {
    shared_ptr_var<audio_engine>(env, self, "audioEngine")->volume(volume);
}

OBOEAUDIODEVICE_METHOD(void, writeSamples) (JNIEnv* env, jobject self, jshortArray data, jint offset, jint samples) {
    auto engine = shared_ptr_var<audio_engine>(env, self, "audioEngine");

    std::vector<int16_t> pcm(samples);
    env->GetShortArrayRegion(data, offset, samples, pcm.data());
    engine->play(pcm);
}

OBOEAUDIODEVICE_METHOD(void, writeSamples) (JNIEnv* env, jobject self, jfloatArray data, jint offset, jint samples) {
    auto engine = shared_ptr_var<audio_engine>(env, self, "audioEngine");

    std::vector<float> pcm(samples);
    env->GetFloatArrayRegion(data, offset, samples, pcm.data());
    engine->play(pcm);
}

OBOEAUDIODEVICE_METHOD(void, dispose) (JNIEnv* env, jobject self) {
    delete get_var_as<std::shared_ptr<audio_engine>>(env, self, "audioEngine");
}

OBOEAUDIODEVICE_METHOD(bool, isMono) (JNIEnv* env, jobject self) {
    return shared_ptr_var<audio_engine>(env, self, "audioEngine")->is_mono();
}
