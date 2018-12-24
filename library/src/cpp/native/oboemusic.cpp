#include "oboemusic.hpp"
#include "../audio/audioengine.hpp"
#include "../music/music.hpp"
#include "../utility/var.hpp"
#include "../jni/jvm_class.hpp"

OBOEMUSIC_METHOD(void, setCompletionCallback) (JNIEnv* env, jobject self, jobject callback) {
    JavaVM* jvm;
    env->GetJavaVM(&jvm);

    auto old_callback = get_var_as<_jobject>(env, self, "onComplete");
    if (old_callback != NULL) {
        env->DeleteGlobalRef(old_callback) ;
    }

    auto new_callback = env->NewGlobalRef(callback);
    set_var_as(env, self, "onComplete", new_callback);

    auto instance = get_var_as<music>(env, self, "music");
    instance->on_complete([new_callback, jvm] {
        auto context = jni_context(jvm);
        auto scoped_env = context.acquire_thread();

        auto callback_class = jvm_class(context, scoped_env->GetObjectClass(new_callback));
        callback_class.execute_method<void()>(new_callback, "invoke");
    });
}

OBOEMUSIC_METHOD(bool, isPlaying) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->is_playing();
}

OBOEMUSIC_METHOD(bool, isLooping) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->is_looping();
}

OBOEMUSIC_METHOD(void, pause) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->pause();
}

OBOEMUSIC_METHOD(void, setPan) (JNIEnv* env, jobject self, jfloat pan, jfloat volume) {
    auto instance = get_var_as<music>(env, self, "music");
    instance->pan(pan);
    instance->volume(volume);
}

OBOEMUSIC_METHOD(jfloat, getPosition) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->position();
}

OBOEMUSIC_METHOD(void, setLooping) (JNIEnv* env, jobject self, jboolean loop) {
    return get_var_as<music>(env, self, "music")->is_looping(loop);
}

OBOEMUSIC_METHOD(jfloat, getVolume) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->volume();
}

OBOEMUSIC_METHOD(void, play) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->play();
}

OBOEMUSIC_METHOD(void, stop) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->stop();
}

OBOEMUSIC_METHOD(void, setVolume) (JNIEnv* env, jobject self, jfloat volume) {
    return get_var_as<music>(env, self, "music")->volume(volume);
}

OBOEMUSIC_METHOD(void, setPosition) (JNIEnv* env, jobject self, jfloat position) {
    get_var_as<music>(env, self, "music")->position(position);
}

OBOEMUSIC_METHOD(void, dispose) (JNIEnv* env, jobject self) {
    delete get_var_as<music>(env, self, "music");
    delete get_var_as<jobject>(env, self, "onComplete");
}

