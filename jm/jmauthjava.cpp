#include <jni.h>
#include <jm/jmlib.h>

JNIEXPORT void Java_jm_auth_Auth_nativeWriteRegister(JNIEnv *env, jclass cls, jstring reg)
{
    const char* str = (const char*)(env->GetStringChars(reg, NULL));
    jm_auth_save_reg(str);
}
