#include <jni.h>
#include "jmui.h"

JNIEXPORT jint Java_jm_ui_GUI_nativePopMsgType(JNIEnv *env, jclass cls) {
    JMUIMsgType type = jm_ui_pop_msg_type();
    return (jint)type;
}

JNIEXPORT jstring Java_jm_ui_GUI_nativePopMsg(JNIEnv *env, jclass cls) {
    gchar *msg = jm_ui_pop_msg();
    if (msg != NULL) {
        return (*env)->NewStringUTF(env, msg);
    }
    return NULL;
}

JNIEXPORT void Java_jm_ui_GUI_nativeSetBtnClicked(JNIEnv *env, jclass cls, jstring str) {
    const gchar *text = (const gchar*)((*env)->GetStringUTFChars(env, str, NULL));
    jm_ui_set_btn_clicked(text);
}

JNIEXPORT void Java_jm_ui_GUI_nativeSetMenuSelected(JNIEnv *env, jclass cls, jstring str) {
    const gchar *text = (const gchar*)((*env)->GetStringUTFChars(env, str, NULL));
    jm_ui_set_menu_selected(text);
}
