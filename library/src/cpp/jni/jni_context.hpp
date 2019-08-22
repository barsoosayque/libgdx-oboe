#pragma once
#include <jni.h>

/// Have to initialize this like this:
/// ```
/// JavaVM* jni_context::s_jvm = 0;
/// JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void *reserved) {
///    jni_context::init(jvm);
///    return JNI_VERSION_1_6;
/// }
/// ```
class jni_context {
    public:
        // RAII JNIEnv* for thread-safe operations
        class scoped_env {
            public:
                scoped_env(JavaVM* p_jvm) : m_jvm(p_jvm) {
                    if (m_jvm->GetEnv(reinterpret_cast<void**>(&m_env), JNI_VERSION_1_6) == JNI_EDETACHED) {
                        m_jvm->AttachCurrentThread(&m_env, NULL);
                        m_attached = true;
                    } else {
                        m_attached = false;
                    }
                }
                ~scoped_env() {
                    m_env = nullptr;
                    if(m_attached) {
                        m_jvm->DetachCurrentThread();
                        m_attached = false;
                    }
                }

                JNIEnv* const operator*() const { return m_env; }
                JNIEnv* const operator->() const { return m_env; }
            private:
                bool m_attached;
                JNIEnv* m_env;
                JavaVM* m_jvm;
        };

        static scoped_env acquire_thread() { return scoped_env(s_jvm); }

        static void init(JavaVM* p_jvm) {
            s_jvm = p_jvm;
        }
    private:
        static JavaVM* s_jvm;
};