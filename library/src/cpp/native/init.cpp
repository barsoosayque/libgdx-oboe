#include "../jni/jni_context.hpp"
#include "../jni/jvm_class.hpp"
#include "../jni/jvm_definitions.hpp"

JavaVM* jni_context::s_jvm = 0;
jvm_class::class_cache_map jvm_class::g_class_cache;
jvm_class::field_cache_map jvm_class::g_field_cache;
jvm_class::method_cache_map jvm_class::g_method_cache;
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void *reserved) {
    jni_context::init(jvm);

    // jvm caching
    jvm_class::cache_class("barsoosayque/libgdxoboe/AudioDecoder");
    jvm_class::cache_class("barsoosayque/libgdxoboe/AudioDecoder$Pcm");
    jvm_class::cache_field<int>("barsoosayque/libgdxoboe/AudioDecoder$Pcm", "size");
    jvm_class::cache_field<ByteBuffer>("barsoosayque/libgdxoboe/AudioDecoder$Pcm", "data");
    jvm_class::cache_field<bool>("barsoosayque/libgdxoboe/AudioDecoder$Pcm", "isLast");
    jvm_class::cache_method<void(AssetFileDescriptor)>("barsoosayque/libgdxoboe/AudioDecoder", "<init>");
    jvm_class::cache_method<Pcm(int)>("barsoosayque/libgdxoboe/AudioDecoder", "decode");
    jvm_class::cache_method<Pcm()>("barsoosayque/libgdxoboe/AudioDecoder", "decode");
    jvm_class::cache_method<void(float)>("barsoosayque/libgdxoboe/AudioDecoder", "seek");

    return JNI_VERSION_1_6;
}
