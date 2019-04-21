#include "oboeaudiodevice.hpp"
#include "../audio/audioengine.hpp"
#include "../utility/var.hpp"

OBOEAUDIODEVICE_METHOD(jint, getLatency) (JNIEnv* env, jobject self) {

}

OBOEAUDIODEVICE_METHOD(void, setVolume) (JNIEnv* env, jobject self, jfloat) {

}

OBOEAUDIODEVICE_METHOD(void, writeSamples) (JNIEnv* env, jobject self, jshortArray, jint, jint) {

}

OBOEAUDIODEVICE_METHOD(void, writeSamples) (JNIEnv* env, jobject self, jfloatArray, jint, jint) {

}

OBOEAUDIODEVICE_METHOD(void, dispose) (JNIEnv* env, jobject self) {
    delete get_var_as<std::shared_ptr<audio_engine>>(env, self, "audioEngine");
}

OBOEAUDIODEVICE_METHOD(bool, isMono) (JNIEnv* env, jobject self) {
    return shared_ptr_var<audio_engine>(env, self, "audioEngine")->is_mono();
}
