#include <jni.h>
#include <glib.h>
#include "jmserialport.h"
#include "jmerror.h"

extern "C"
{
    JNIEXPORT jlong Java_jm_io_SerialPort_nativeNewSerialPort(JNIEnv *env, 
        jclass cls)
    {
        JMSerialPort *port = jm_serial_port_new();
        return (jlong)port;
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetPortName(JNIEnv *env, 
        jclass cls, jlong handle, jstring port_name)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        const gchar *name = NULL;
        name = (const gchar*)( env->GetStringChars(port_name, NULL));
        return (jint)jm_serial_port_set_port_name(port, name);
    }

    JNIEXPORT jstring Java_jm_io_Serial_port_nativePortName(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        const gchar *name = jm_serial_port_get_port_name(port);
        return env->NewStringUTF(name);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetBaudrate(JNIEnv *env, 
        jclass cls, jlong handle, jint baudrate)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_baudrate(port, baudrate);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeBaudrate(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_get_baudrate(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetDatabits(JNIEnv *env, 
        jclass cls, jlong handle, jbyte databits)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_databits(port, databits);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeDatabits(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_get_databits(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetParity(JNIEnv *env, 
        jclass cls, jlong handle, jint parity)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_parity(port, parity);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeParity(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_get_parity(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetStopbits(JNIEnv *env, 
        jclass cls, jlong handle, jint stopbits)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_stopbits(port, stopbits);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeStopbits(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_get_stopbits(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetFlowControl(JNIEnv *env, 
        jclass cls, jlong handle, jint flow)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_flow_control(port, flow);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeFlowControl(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_get_flow_control(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetReadTimeout(JNIEnv *env, 
        jclass cls, jlong handle, jlong millic_seconds)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_read_timeout(port, millic_seconds);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetWriteTimeout(JNIEnv *env, 
        jclass cls, jlong handle, jlong millic_seconds)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_write_timeout(port, millic_seconds);
    }

    JNIEXPORT jboolean Java_jm_io_SerialPort_nativeIsOpen(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_is_open(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeOpen(JNIEnv *env, jclass cls, 
        jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_open(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeClose(JNIEnv *env, jclass cls, 
        jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_close(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeFlush(JNIEnv *env, jclass cls, 
        jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_flush(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeDiscardInBuffer(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_discard_in_buffer(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeDiscardOutBuffer(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_discard_out_buffer(port);
    }

    JNIEXPORT jlong Java_jm_io_SerialPort_nativeBytesAvailable(JNIEnv *env, 
        jclass cls, jlong handle)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_bytes_available(port);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetDtr(JNIEnv *env, jclass cls, 
        jlong handle, jboolean set)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_dtr(port, set);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeSetRts(JNIEnv *env, jclass cls, 
        jlong handle, jboolean set)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_set_rts(port, set);
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeRead(JNIEnv *env, jclass cls, 
        jlong handle, jbyteArray buff, jint offset, jint count)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        guint8 *temp = (guint8*)g_malloc(count);
        size_t ret = jm_serial_port_read(port, temp, count);
        if (ret == count)
        {
            env->SetByteArrayRegion(buff, offset, count, 
                (const jbyte*)temp);
        }
        g_free(temp);
        return ret;
    }

    JNIEXPORT jint Java_jm_io_SerialPort_nativeWrite(JNIEnv *env, jclass cls, 
        jlong handle, jbyteArray buff, jint offset, jint count)
    {
        JMSerialPort *port = (JMSerialPort*)handle;
        return jm_serial_port_write(port, (const guint8*)
            (env->GetByteArrayElements(buff, NULL)) + offset, count);
    }

    JNIEXPORT jobjectArray Java_jm_io_SerialPort_getSystemPorts(JNIEnv *env, 
        jclass cls)
    {
        JMStringArray *arr = jm_serial_port_get_system_ports();
        jobjectArray ret = env->NewObjectArray(
            jm_string_array_size(arr), 
            env->FindClass("java/lang/String"), 
            env->NewStringUTF(""));
        size_t i;
        for (i = 0; i < jm_string_array_size(arr); i++)
        {
            env->SetObjectArrayElement(ret, i, 
                env->NewStringUTF(jm_string_array_get(arr, i)));
        }
        return ret;
    }

}
