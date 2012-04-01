#include <jni.h>
#include "jmlib.h"

//JNIEXPORT void Java_jm_io_CommboxPort_nativePushInDeque(JNIEnv *env, jclass cls, jbyteArray buff, jint count)
//{
//	jm_commbox_port_push_in_deque((const uint8_t*)env->GetByteArrayElements(buff, NULL), count);
//}
//
//JNIEXPORT jint Java_jm_io_CommboxPort_nativeOutDequeAvailable(JNIEnv *env, jclass cls)
//{
//	return jm_commbox_port_out_deque_available();
//}
//
//JNIEXPORT jboolean Java_jm_io_CommboxPort_nativePopOutDeque(JNIEnv *env, jclass cls, jbyteArray buff, jint count)
//{
//	return jm_commbox_port_pop_out_deque((uint8_t*)env->GetByteArrayElements(buff, NULL), count) ? JNI_TRUE : JNI_FALSE;
//}