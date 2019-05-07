#include "oboeaudiorecorder.cpp.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"

OBOEAUDIORECORDER_METHOD(void, read) (JNIEnv* env, jobject self, jshortArray, jint, jint) {
    // TODO
}

OBOEAUDIORECORDER_METHOD(void, dispose) (JNIEnv* env, jobject self) {
    delete get_var_as<std::shared_ptr<audio_engine>>(env, self, "audioEngine");
}
