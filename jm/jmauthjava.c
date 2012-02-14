#include <jni.h>
#include "jmauth.h"

JNIEXPORT void Java_jm_auth_Auth_nativeWriteRegister(JNIEnv *env, jclass cls, jstring reg) {
    const gchar* str = (const gchar*)(*env)->GetStringChars(env, reg, NULL);
    jm_auth_write_register(str);
}
