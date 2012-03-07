#include <jni.h>
#include "jmui.h"

JNIEXPORT jobject Java_jm_ui_GUI_nativePopMsg(JNIEnv *env, jclass cls)
{
    JMUIMessage *msg = jm_ui_pop_msg();
    jclass class_GUIMessage = NULL;
    jmethodID id_GUIMessage = NULL;
    jobject obj_GUIMessage = NULL;
    if (msg == NULL)
        return NULL;

    class_GUIMessage = (*env)->FindClass(env, "jm/gui/Message");
    id_GUIMessage = (*env)->GetMethodID(env, class_GUIMessage, "<init>", 
        "(ILjava/lang/String;)V");
    obj_GUIMessage = (*env)->NewObject(env, class_GUIMessage, 
        id_GUIMessage, msg->type, (*env)->NewStringUTF(env, msg->message));
    jm_ui_msg_free(msg);
    return obj_GUIMessage;
}

#if 0
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

JNIEXPORT void Java_jm_ui_GUI_nativeSetBtnClicked(JNIEnv *env, jclass cls, 
    jstring str) {
    const gchar *text = (const gchar*)((*env)->GetStringUTFChars(env, str, 
        NULL));
    jm_ui_set_btn_clicked(text);
}

JNIEXPORT void Java_jm_ui_GUI_nativeSetMenuSelected(JNIEnv *env, 
    jclass cls, jstring str) {
    const gchar *text = (const gchar*)((*env)->GetStringUTFChars(env, str, 
        NULL));
    jm_ui_set_menu_selected(text);
}

#endif