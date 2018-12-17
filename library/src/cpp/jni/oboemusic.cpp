#include "oboemusic.hpp"
#include "../audio/audioengine.hpp"
#include "../music/music.hpp"
#include "../utility/var.hpp"

OBOEMUSIC_METHOD(bool, isPlaying) (JNIEnv* env, jobject self) {

}

OBOEMUSIC_METHOD(bool, isLooping) (JNIEnv* env, jobject self) {

}

OBOEMUSIC_METHOD(void, pause) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->pause();
}

OBOEMUSIC_METHOD(void, setPan) (JNIEnv* env, jobject self, jfloat, jfloat) {

}

OBOEMUSIC_METHOD(jfloat, getPosition) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->position();
}

OBOEMUSIC_METHOD(void, setLooping) (JNIEnv* env, jobject self, jfloat) {

}

OBOEMUSIC_METHOD(jfloat, getVolume) (JNIEnv* env, jobject self) {

}

OBOEMUSIC_METHOD(void, play) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->play();
}

OBOEMUSIC_METHOD(void, stop) (JNIEnv* env, jobject self) {
    return get_var_as<music>(env, self, "music")->stop();
}

OBOEMUSIC_METHOD(void, setVolume) (JNIEnv* env, jobject self, jfloat) {

}

OBOEMUSIC_METHOD(void, setPosition) (JNIEnv* env, jobject self, jfloat position) {
    get_var_as<music>(env, self, "music")->position(position);
}

OBOEMUSIC_METHOD(void, dispose) (JNIEnv* env, jobject self) {

}

