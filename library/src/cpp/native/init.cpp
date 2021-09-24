#include "../jni/jni_context.hpp"
#include "../jni/jvm_class.hpp"

JavaVM *jni_context::s_jvm = nullptr;
jvm_class::class_cache_map jvm_class::g_class_cache;
jvm_class::field_cache_map jvm_class::g_field_cache;
jvm_class::method_cache_map jvm_class::g_method_cache;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    jni_context::init(jvm);
    return JNI_VERSION_1_6;
}
