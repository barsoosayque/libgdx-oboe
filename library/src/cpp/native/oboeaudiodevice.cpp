#include "oboeaudiodevice.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"

OBOEAUDIODEVICE_METHOD(jint, getLatency)(JNIEnv *env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        return engine->payload_size();
    }
    return 0;
}

OBOEAUDIODEVICE_METHOD(void, setVolume)(JNIEnv *env, jobject self, jfloat volume) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        engine->volume(volume);
    }
}

OBOEAUDIODEVICE_METHOD(void, writeSamples___3SII)(JNIEnv *env, jobject self, jshortArray data,
                                                  jint offset, jint samples) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        std::vector<int16_t> pcm(samples);
        env->GetShortArrayRegion(data, offset, samples, pcm.data());
        engine->play(pcm);
    }
}

OBOEAUDIODEVICE_METHOD(void, writeSamples___3FII)(JNIEnv *env, jobject self, jfloatArray data,
                                                  jint offset, jint samples) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        std::vector<float> pcm(samples);
        env->GetFloatArrayRegion(data, offset, samples, pcm.data());
        engine->play(pcm);
    }
}

OBOEAUDIODEVICE_METHOD(void, dispose)(JNIEnv *env, jobject self) {
    delete_var<audio_engine>(env, self, "audioEngine");
}

OBOEAUDIODEVICE_METHOD(bool, isMono)(JNIEnv *env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        return engine->is_mono();
    }
    return false;
}

OBOEAUDIODEVICE_METHOD(void, pause)(JNIEnv *env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        engine->stop();
    }
}

OBOEAUDIODEVICE_METHOD(void, resume)(JNIEnv *env, jobject self) {
    if (auto engine = get_var_as<audio_engine>(env, self, "audioEngine")) {
        engine->resume();
    }
}
