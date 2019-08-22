#include "../jni/jni_context.hpp"

JavaVM* jni_context::s_jvm = 0;
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void *reserved) {
    jni_context::init(jvm);
    return JNI_VERSION_1_6;
}
