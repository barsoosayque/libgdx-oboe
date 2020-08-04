#include "jni.h"
#include <string_view>
#include <memory>

template<class T>
T* get_var_as(JNIEnv* env, jobject obj, std::string_view var) {
    auto object_class = env->GetObjectClass(obj);
    auto var_id = env->GetFieldID(object_class, var.data(), "J");
    return reinterpret_cast<T*>(env->GetLongField(obj, var_id));
}

template<class T>
void set_var_as(JNIEnv* env, jobject obj, std::string_view var, T* value) {
    auto object_class = env->GetObjectClass(obj);
    auto var_id = env->GetFieldID(object_class, var.data(), "J");
    return env->SetLongField(obj, var_id, reinterpret_cast<jlong>(value));
}
template<class T>
void delete_var(JNIEnv* env, jobject obj, std::string_view var) {
    auto instance = get_var_as<T>(env, obj, var);
    delete instance;
    instance = nullptr;
    set_var_as<T>(env, obj, var, nullptr);
}