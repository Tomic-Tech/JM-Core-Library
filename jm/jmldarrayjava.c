#include <jni.h>
#include "jmldarray.h"
#include "jmdb.h"

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetShortName(JNIEnv *env, jclass cls, jint index)
{
    return (*env)->NewStringUTF(env, jm_ld_array_get_short_name(index));
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetContent(JNIEnv *env, jclass cls, jint index)
{
    return (*env)->NewStringUTF(env, jm_ld_array_get_content(index));
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetUnit(JNIEnv *env, jclass cls, jint index)
{
    return (*env)->NewStringUTF(env, jm_ld_array_get_unit(index));
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetDefaultValue(JNIEnv *env, jclass cls, jint index)
{
    return (*env)->NewStringUTF(env, jm_ld_array_get_default_value(index));
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetValue(JNIEnv *env, jclass cls, jint index)
{
    return (*env)->NewStringUTF(env, jm_ld_array_get_value(index));
}

JNIEXPORT jboolean Java_jm_core_LDArray_nativeGetEnabled(JNIEnv *env, jclass cls, jint index)
{
    return jm_ld_array_get_enabled(index) ? JNI_TRUE : JNI_FALSE;
}

