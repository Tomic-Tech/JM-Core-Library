#include <jni.h>
#include "jmldvector.hpp"

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetShortName(JNIEnv *env, jclass cls, jint index)
{
    return env->NewStringUTF(JM::LiveDataVector::globalAt(index)->shortName().c_str());
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetContent(JNIEnv *env, jclass cls, jint index)
{
    return env->NewStringUTF(JM::LiveDataVector::globalAt(index)->content().c_str());
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetUnit(JNIEnv *env, jclass cls, jint index)
{
    return env->NewStringUTF(JM::LiveDataVector::globalAt(index)->unit().c_str());
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetDefaultValue(JNIEnv *env, jclass cls, jint index)
{
    return env->NewStringUTF(JM::LiveDataVector::globalAt(index)->defaultValue().c_str());
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetValue(JNIEnv *env, jclass cls, jint index)
{
    return env->NewStringUTF(JM::LiveDataVector::globalAt(index)->value().c_str());
}

JNIEXPORT jboolean Java_jm_core_LDArray_nativeGetEnabled(JNIEnv *env, jclass cls, jint index)
{
    return JM::LiveDataVector::globalAt(index)->enabled() ? JNI_TRUE : JNI_FALSE;
}

