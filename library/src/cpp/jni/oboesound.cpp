#include "oboesound.hpp"
#include "../sound/soundpool.hpp"
#include "../utility/var.hpp"

OBOESOUND_METHOD(void, pause__) (JNIEnv* env, jobject self) {
    get_var_as<soundpool>(env, self, "soundpool")->pause();
}

OBOESOUND_METHOD(void, pause__J) (JNIEnv* env, jobject self, jlong id) {
    get_var_as<soundpool>(env, self, "soundpool")->pause(id);
}

OBOESOUND_METHOD(jlong, play__) (JNIEnv* env, jobject self) {
    return get_var_as<soundpool>(env, self, "soundpool")->play();
}

OBOESOUND_METHOD(jlong, play__F) (JNIEnv* env, jobject self, jfloat volume) {
    return get_var_as<soundpool>(env, self, "soundpool")->play(volume);
}

OBOESOUND_METHOD(void, stop__) (JNIEnv* env, jobject self) {
    get_var_as<soundpool>(env, self, "soundpool")->stop();
}

OBOESOUND_METHOD(void, stop__J) (JNIEnv* env, jobject self, jlong id) {
    get_var_as<soundpool>(env, self, "soundpool")->stop(id);
}

OBOESOUND_METHOD(void, resume__) (JNIEnv* env, jobject self) {
    get_var_as<soundpool>(env, self, "soundpool")->resume();
}

OBOESOUND_METHOD(void, resume__J) (JNIEnv* env, jobject self, jlong id) {
    get_var_as<soundpool>(env, self, "soundpool")->resume(id);
}

OBOESOUND_METHOD(void, setVolume) (JNIEnv* env, jobject self, jlong id, jfloat volume) {

}

OBOESOUND_METHOD(void, dispose) (JNIEnv* env, jobject self) {

}

