#include "oboemusic.hpp"
#include "../audio/audioengine.hpp"
#include "../music/music.hpp"
#include "../utility/var.hpp"
#include "../jni/jvm_class.hpp"

OBOEMUSIC_METHOD(void, setCompletionCallback) (JNIEnv* env, jobject self, jobject callback) {
//    auto context = jni_context::acquire_thread();
//
//    if (auto old_callback = get_var_as<_jobject>(env, self, "onComplete")) {
//        context->DeleteGlobalRef(old_callback);
//    }
//
//    if (auto instance = shared_ptr_var<music>(env, self, "music")) {
//        auto new_callback = context->NewGlobalRef(callback);
//        set_var_as(env, self, "onComplete", new_callback);
//
//        instance->on_complete([new_callback] {
//            auto context = jni_context::acquire_thread();
//            auto callback_class = jvm_class(context->GetObjectClass(new_callback));
//            callback_class.execute_method<void()>(new_callback, "invoke");
//        });
//    }
}

OBOEMUSIC_METHOD(bool, isPlaying) (JNIEnv* env, jobject self) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        return instance->is_playing();
    }
    return false;
}

OBOEMUSIC_METHOD(bool, isLooping) (JNIEnv* env, jobject self) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        return instance->is_looping();
    }
    return false;
}

OBOEMUSIC_METHOD(void, pause) (JNIEnv* env, jobject self) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        instance->pause();
    }
}

OBOEMUSIC_METHOD(void, setPan) (JNIEnv* env, jobject self, jfloat pan, jfloat volume) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        instance->pan(pan);
        instance->volume(volume);
    }
}

OBOEMUSIC_METHOD(jfloat, getPosition) (JNIEnv* env, jobject self) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        return instance->position();
    }
    return 0;
}

OBOEMUSIC_METHOD(void, setLooping) (JNIEnv* env, jobject self, jboolean loop) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        instance->is_looping(loop);
    }
}

OBOEMUSIC_METHOD(jfloat, getVolume) (JNIEnv* env, jobject self) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        return instance->volume();
    }
    return 0;
}

OBOEMUSIC_METHOD(void, play) (JNIEnv* env, jobject self) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        instance->play();
    }
}

OBOEMUSIC_METHOD(void, stop) (JNIEnv* env, jobject self) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        instance->stop();
    }
}

OBOEMUSIC_METHOD(void, setVolume) (JNIEnv* env, jobject self, jfloat volume) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        instance->volume(volume);
    }
}

OBOEMUSIC_METHOD(void, setPosition) (JNIEnv* env, jobject self, jfloat position) {
    if(auto instance = get_var_as<music>(env, self, "music")) {
        instance->position(position);
    }
}

OBOEMUSIC_METHOD(void, dispose) (JNIEnv* env, jobject self) {
//    if (auto callback = get_var_as<_jobject>(env, self, "onComplete")) {
//        auto context = jni_context::acquire_thread();
//        context->DeleteGlobalRef(callback);
//        set_var_as<_jobject>(env, self, "onComplete", nullptr);
//    }
//    delete get_var_as<std::shared_ptr<music>>(env, self, "music");
}

