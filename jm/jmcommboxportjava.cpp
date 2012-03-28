#include <jni.h>
#include "jmcommboxport.hpp"

JNIEXPORT void Java_jm_io_CommboxPort_nativePushInDeque(JNIEnv *env, jclass cls, jbyteArray buff, jint count)
{
	JM::CommboxPort::inst().pushInDeque(boost::asio::const_buffer(env->GetByteArrayElements(buff, NULL), count));
}

JNIEXPORT jint Java_jm_io_CommboxPort_nativeOutDequeAvailable(JNIEnv *env, jclass cls)
{
	jint ret = JM::CommboxPort::inst().outDequeAvailable();
	return ret;
}

JNIEXPORT jboolean Java_jm_io_CommboxPort_nativePopOutDeque(JNIEnv *env, jclass cls, jbyteArray buff, jint count)
{
	jboolean ret = JM::CommboxPort::inst().popOutDeque(boost::asio::mutable_buffer(env->GetByteArrayElements(buff, NULL), count));
	return ret;
}