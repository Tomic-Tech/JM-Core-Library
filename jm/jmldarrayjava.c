#include <jni.h>
#include "jmldarray.h"
#include "jmdb.h"

JNIEXPORT jlong Java_jm_core_LDArray_nativeGetLDArray(JNIEnv *env, jclass cls) {
    jlong ret = (jlong)jm_db_get_live_data();
    return ret;
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetShortName(JNIEnv *env, jclass cls, jlong handle, jint index) {
    JMLDArray *arr = (JMLDArray*)handle;
    JMLiveData *ld = NULL;
    if (arr == NULL) {
        return NULL;
    }

    ld = jm_ld_array_index(arr, index);
    if (ld == NULL) {
        return NULL;
    }
    return (*env)->NewStringUTF(env, ld->short_name);
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetContent(JNIEnv *env, jclass cls, jlong handle, jint index) {
    JMLDArray *arr = (JMLDArray*)handle;
    JMLiveData *ld = NULL;
    if (arr == NULL) {
        return NULL;
    }

    ld = jm_ld_array_index(arr, index);
    if (ld == NULL) {
        return NULL;
    }
    return (*env)->NewStringUTF(env, ld->content);
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetUnit(JNIEnv *env, jclass cls, jlong handle, jint index) {
    JMLDArray *arr = (JMLDArray*)handle;
    JMLiveData *ld = NULL;
    if (arr == NULL) {
        return NULL;
    }

    ld = jm_ld_array_index(arr, index);
    if (ld == NULL) {
        return NULL;
    }
    return (*env)->NewStringUTF(env, ld->unit);
}

JNIEXPORT jstring Java_jm_core_LDArray_nativeGetDefaultValue(JNIEnv *env, jclass cls, jlong handle, jint index) {
    JMLDArray *arr = (JMLDArray*)handle;
    JMLiveData *ld = NULL;
    if (arr == NULL) {
        return NULL;
    }

    ld = jm_ld_array_index(arr, index);
    if (ld == NULL) {
        return NULL;
    }
    return (*env)->NewStringUTF(env, ld->default_value);
}

JNIEXPORT jboolean Java_jm_core_LDArray_nativeGetEnabled(JNIEnv *env, jclass cls, jlong handle, jint index) {
    JMLDArray *arr = (JMLDArray*)handle;
    JMLiveData *ld = NULL;
    if (arr == NULL) {
        return JNI_FALSE;
    }

    ld = jm_ld_array_index(arr, index);
    if (ld == NULL) {
        return JNI_FALSE;
    }

    return ld->enabled ? JNI_TRUE : JNI_FALSE;
}

