#include "oboemusic.hpp"
#include "../audio/audioengine.hpp"
#include "../music/music.hpp"
#include "../utility/var.hpp"
#include "../jni/jvm_class.hpp"
#include "external_classes.hpp"

OBOEMUSIC_METHOD(void, init)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        auto self_weak = env->NewWeakGlobalRef(self);
        (*instance)->on_complete([self_weak] {
            auto ctx = jni_context::acquire_thread();
            auto listener = get_jvm_var<external::gdx_music_listener>(*ctx, self_weak,
                                                                      "listener");
            if (listener.obj) {
                auto cls = jvm_class(external::gdx_music_listener::cls.data());
                cls.execute_method<void(external::gdx_music)>(listener.obj, "onCompletion",
                                                              external::gdx_music{ self_weak });
            }
        });
    }
}

OBOEMUSIC_METHOD(bool, isPlaying)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        return (*instance)->is_playing();
    }
    return false;
}

OBOEMUSIC_METHOD(bool, isLooping)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        return (*instance)->is_looping();
    }
    return false;
}

OBOEMUSIC_METHOD(void, pause)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        (*instance)->pause();
    }
}

OBOEMUSIC_METHOD(void, setPan)(JNIEnv *env, jobject self, jfloat pan, jfloat volume) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        (*instance)->pan(pan);
        (*instance)->volume(volume);
    }
}

OBOEMUSIC_METHOD(jfloat, getPosition)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        return (*instance)->position();
    }
    return 0;
}

OBOEMUSIC_METHOD(void, setLooping)(JNIEnv *env, jobject self, jboolean loop) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        (*instance)->is_looping(loop);
    }
}

OBOEMUSIC_METHOD(jfloat, getVolume)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        return (*instance)->volume();
    }
    return 0;
}

OBOEMUSIC_METHOD(void, play)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        (*instance)->play();
    }
}

OBOEMUSIC_METHOD(void, stop)(JNIEnv *env, jobject self) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        (*instance)->stop();
    }
}

OBOEMUSIC_METHOD(void, setVolume)(JNIEnv *env, jobject self, jfloat volume) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        (*instance)->volume(volume);
    }
}

OBOEMUSIC_METHOD(void, setPosition)(JNIEnv *env, jobject self, jfloat position) {
    if (auto instance = get_var_as<std::shared_ptr<music>>(env, self, "music")) {
        (*instance)->position(position);
    }
}

OBOEMUSIC_METHOD(void, dispose)(JNIEnv *env, jobject self) {
    set_jvm_var<external::gdx_music_listener>(env, self, "listener", nullptr);
    delete_var<std::shared_ptr<music>>(env, self, "music");
}

