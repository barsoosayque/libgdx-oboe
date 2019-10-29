#include "../jni/jni_context.hpp"
#include "../jni/jvm_class.hpp"

JavaVM* jni_context::s_jvm = 0;
std::unordered_map<std::size_t, jclass> jvm_class::g_class_cache;
std::unordered_map<std::size_t, std::unordered_map<std::size_t, jfieldID>> jvm_class::g_field_cache;
std::unordered_map<std::size_t, std::unordered_map<std::size_t, jmethodID>> jvm_class::g_method_cache;
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void *reserved) {
    jni_context::init(jvm);
    return JNI_VERSION_1_6;
}
