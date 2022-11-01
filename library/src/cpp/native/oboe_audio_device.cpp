#include "oboe_audio_device.hpp"
#include "../audio/audio_stream.hpp"
#include "../utility/var.hpp"

namespace {
constexpr std::string_view k_var = "audioStream";
}

OBOEAUDIODEVICE_METHOD(jint, getLatency)(JNIEnv *env, jobject self) {
    if (auto stream = get_var_as<audio_stream>(env, self, k_var)) {
        return static_cast<jint>(stream->latency());
    }
    return 0;
}

OBOEAUDIODEVICE_METHOD(void, setVolume)(JNIEnv *env, jobject self, jfloat volume) {
    if (auto stream = get_var_as<audio_stream>(env, self, k_var)) {
        stream->volume(volume);
    }
}

OBOEAUDIODEVICE_METHOD(void, writeSamples___3SII)(JNIEnv *env, jobject self, jshortArray data,
                                                  jint offset, jint samples) {
    if (auto stream = get_var_as<audio_stream>(env, self, k_var)) {
        std::vector<int16_t> pcm(samples);
        env->GetShortArrayRegion(data, offset, samples, pcm.data());
        stream->write(std::move(pcm));
    }
}

OBOEAUDIODEVICE_METHOD(void, writeSamples___3FII)(JNIEnv *env, jobject self, jfloatArray data,
                                                  jint offset, jint samples) {
    if (auto stream = get_var_as<audio_stream>(env, self, k_var)) {
        std::vector<float> pcm(samples);
        env->GetFloatArrayRegion(data, offset, samples, pcm.data());
        stream->write(std::move(pcm));
    }
}

OBOEAUDIODEVICE_METHOD(void, dispose)(JNIEnv *env, jobject self) {
    delete_var<audio_stream>(env, self, k_var);
}

OBOEAUDIODEVICE_METHOD(bool, isMono)(JNIEnv *env, jobject self) {
    if (auto stream = get_var_as<audio_stream>(env, self, k_var)) {
        return stream->is_mono();
    }
    return false;
}

OBOEAUDIODEVICE_METHOD(void, pause)(JNIEnv *env, jobject self) {
    if (auto stream = get_var_as<audio_stream>(env, self, k_var)) {
        stream->stop();
    }
}

OBOEAUDIODEVICE_METHOD(void, resume)(JNIEnv *env, jobject self) {
    if (auto stream = get_var_as<audio_stream>(env, self, k_var)) {
        stream->resume();
    }
}
