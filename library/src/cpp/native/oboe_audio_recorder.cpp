#include "oboe_audio_recorder.hpp"
#include "../utility/var.hpp"
#include "../utility/log.hpp"
#include "../audio/audio_recorder.hpp"

namespace {
constexpr std::string_view k_var = "audioRecorder";
}

OBOEAUDIORECORDER_METHOD(void, read)(JNIEnv *env, jobject self, jshortArray data, jint offset, jint samples) {
    if (auto recorder = get_var_as<audio_recorder>(env, self, k_var)) {
        auto* begin = env->GetShortArrayElements(data, nullptr) + offset;
        auto* end = begin + samples;
        recorder->read(begin, end);
        env->ReleaseShortArrayElements(data, begin, 0);
    }
}

OBOEAUDIORECORDER_METHOD(void, dispose)(JNIEnv *env, jobject self) {
    delete get_var_as<std::shared_ptr<audio_recorder>>(env, self, k_var);
}
