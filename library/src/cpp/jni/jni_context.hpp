#pragma once
#include <jni.h>

class jni_context {
    public:
        jni_context(JNIEnv* p_env) { p_env->GetJavaVM(&m_jvm); }

        // RAII JNIEnv* for multi-threading
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

        JNIEnv* const operator->() const {
            JNIEnv* env;
            assert(JNI_OK == m_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6));
            return env;
        }
        scoped_env acquire_thread() const { return scoped_env(m_jvm); }
    private:
        JavaVM* m_jvm;
};