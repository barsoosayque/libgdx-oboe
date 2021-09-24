#include "oboesound.hpp"
#include "../sound/soundpool.hpp"
#include "../utility/var.hpp"

OBOESOUND_METHOD(jlong, play)(JNIEnv *env, jobject self, jfloat volume, jfloat pitch, jfloat pan) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        return (*instance)->play(volume, pitch, pan, false);
    }
    return 0;
}

OBOESOUND_METHOD(jlong, loop)(JNIEnv *env, jobject self, jfloat volume, jfloat pitch, jfloat pan) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        return (*instance)->play(volume, pitch, pan, true);
    }
    return 0;
}

OBOESOUND_METHOD(void, pause__)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->pause();
    }
}

OBOESOUND_METHOD(void, pause__J)(JNIEnv *env, jobject self, jlong id) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->pause(id);
    }
}

OBOESOUND_METHOD(void, stop__)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->stop();
    }
}

OBOESOUND_METHOD(void, stop__J)(JNIEnv *env, jobject self, jlong id) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->stop(id);
    }
}

OBOESOUND_METHOD(void, resume__)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->resume();
    }
}

OBOESOUND_METHOD(void, resume__J)(JNIEnv *env, jobject self, jlong id) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->resume(id);
    }
}

OBOESOUND_METHOD(void, setLooping)(JNIEnv *env, jobject self, jlong id, jboolean loop) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->looping(id, loop);
    }
}

OBOESOUND_METHOD(void, setVolume)(JNIEnv *env, jobject self, jlong id, jfloat volume) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->volume(id, volume);
    }
}

OBOESOUND_METHOD(void, dispose)(JNIEnv *env, jobject self) {
    delete_var<std::shared_ptr<soundpool>>(env, self, "soundpool");
}

OBOESOUND_METHOD(void, setPitch)(JNIEnv *env, jobject self, jlong id, jfloat pitch) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->speed(id, pitch);
    }
}

OBOESOUND_METHOD(void, setPan)(JNIEnv *env, jobject self, jlong id, jfloat pan, jfloat volume) {
    if (auto instance = get_var_as<std::shared_ptr<soundpool>>(env, self, "soundpool")) {
        (*instance)->pan(id, pan);
        (*instance)->volume(id, volume);
    }
}
