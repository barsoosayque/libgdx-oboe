#include "manaudiosound.hpp"
#include "../sound/soundpool.hpp"
#include "../utility/var.hpp"

MANAUDIO_SOUND_METHOD(void, pause__) (JNIEnv* env, jobject self) {

}

MANAUDIO_SOUND_METHOD(void, pause__J) (JNIEnv* env, jobject self, jlong id) {

}

MANAUDIO_SOUND_METHOD(jlong, play__) (JNIEnv* env, jobject self) {
    return get_var_as<soundpool>(env, self, "soundpool")->play();
}

MANAUDIO_SOUND_METHOD(jlong, play__F) (JNIEnv* env, jobject self, jfloat volume) {
    return 0;
}

MANAUDIO_SOUND_METHOD(void, stop__) (JNIEnv* env, jobject self) {

}

MANAUDIO_SOUND_METHOD(void, stop__J) (JNIEnv* env, jobject self, jlong id) {

}

MANAUDIO_SOUND_METHOD(void, resume__) (JNIEnv* env, jobject self) {

}

MANAUDIO_SOUND_METHOD(void, resume__J) (JNIEnv* env, jobject self, jlong id) {

}

MANAUDIO_SOUND_METHOD(void, setVolume) (JNIEnv* env, jobject self, jlong id, jfloat volume) {

}

MANAUDIO_SOUND_METHOD(void, dispose) (JNIEnv* env, jobject self) {

}

