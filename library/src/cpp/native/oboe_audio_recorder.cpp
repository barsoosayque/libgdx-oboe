#include "oboe_audio_recorder.hpp"
#include "../audio/oboe_engine.hpp"
#include "../utility/var.hpp"

OBOEAUDIORECORDER_METHOD(void, read)(JNIEnv *env, jobject self, jshortArray samples, jint offset, jint numSamples) {
    // TODO
}

OBOEAUDIORECORDER_METHOD(void, dispose)(JNIEnv *env, jobject self) {
//    delete get_var_as<std::shared_ptr <oboe_engine>>
//    (env, self, "audioEngine");
}
