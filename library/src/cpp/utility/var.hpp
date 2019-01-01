#include "jni.h"
#include <string_view>
#include <memory>

template<class T>
T* get_var_as(JNIEnv* p_env, jobject p_obj, std::string_view p_var) {
    auto object_class = p_env->GetObjectClass(p_obj);
    auto var_id = p_env->GetFieldID(object_class, p_var.data(), "J");
    return reinterpret_cast<T*>(p_env->GetLongField(p_obj, var_id));
}

template<class T>
std::shared_ptr<T> shared_ptr_var(JNIEnv* p_env, jobject p_obj, std::string_view p_var) {
    auto object_class = p_env->GetObjectClass(p_obj);
    auto var_id = p_env->GetFieldID(object_class, p_var.data(), "J");
    return *reinterpret_cast<std::shared_ptr<T>*>(p_env->GetLongField(p_obj, var_id));
}

template<class T>
void set_var_as(JNIEnv* p_env, jobject p_obj, std::string_view p_var, T* p_value) {
    auto object_class = p_env->GetObjectClass(p_obj);
    auto var_id = p_env->GetFieldID(object_class, p_var.data(), "J");
    return p_env->SetLongField(p_obj, var_id, reinterpret_cast<jlong>(p_value));
}
