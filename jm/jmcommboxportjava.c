#include <jni.h>
#include "jmcommboxport.h"

JNIEXPORT void Java_jm_io_CommboxPort_nativePushInDeque(JNIEnv *env, jclass cls, jbyteArray buff, jint count) {

    jm_commbox_port_push_in_deque((const guint8*)((*env)->GetByteArrayElements(env, buff, NULL)), count);
}

JNIEXPORT jboolean Java_jm_io_CommboxPort_nativeOutDequeAvailable(JNIEnv *env, jclass cls) {
    jboolean ret = jm_commbox_port_out_deque_available() ? JNI_TRUE : JNI_FALSE;
    return ret;
}

JNIEXPORT jbyteArray Java_jm_io_CommboxPort_nativePopOutDeque(JNIEnv *env, jclass cls) {
    GByteArray *data = NULL;
    jbyteArray ret = NULL;
    if (jm_commbox_port_pop_out_deque(&data)) {
        ret = (*env)->NewByteArray(env, data->len);
        (*env)->SetByteArrayRegion(env, ret, 0, data->len, (jbyte*)(data->data));
        g_byte_array_free(data, TRUE);
    }

    return ret;
}