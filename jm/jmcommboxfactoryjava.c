#include <jni.h>
#include "jmcommboxfactory.h"

JNIEXPORT void Java_io_Commbox_nativeSetVersion(JNIEnv *env, jclass cls, jint ver) {
    jm_commbox_factory_set_commbox_version((JMCommboxVersion)ver);
}