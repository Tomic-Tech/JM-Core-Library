#include <jni.h>
#include "jmlib.h"

JNIEXPORT void Java_jm_core_Core_nativeInitialize(JNIEnv *env, jclass cls, jstring path)
{
    const char* software_path = (const char*)(env->GetStringUTFChars(path, NULL));
    jm_lib_init(software_path);
}

JNIEXPORT void Java_jm_core_Core_nativeDestroy(JNIEnv *env, jclass cls, jstring path)
{
    jm_lib_destroy();
}