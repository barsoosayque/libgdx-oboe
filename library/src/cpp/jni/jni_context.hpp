#pragma once
#include <jni.h>

class jni_context {
    public:
        jni_context(JNIEnv* p_env) {
            p_env->GetJavaVM(&m_jvm);
        }

        JNIEnv* env() {
            JNIEnv* env;
            m_jvm->AttachCurrentThread(&env, NULL);
            return env;
        }

        JNIEnv* operator->() {
            return env();
        }
    private:
        JavaVM* m_jvm;
};