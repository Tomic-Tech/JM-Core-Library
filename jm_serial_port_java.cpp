#include <jni.h>
#include <boost/smart_ptr.hpp>
#include "jm_serial_port.hpp"

using namespace std;
using namespace jm;

extern "C" {
    jlong Java_jm_lib_SerialPort_nativeNewSerialPort(JNIEnv *env, jclass cls) {
        serial_port *port = new serial_port();
        return (jlong)port;
    }
    
    jint Java_jm_lib_SerialPort_nativeSetPortName(JNIEnv *env, jclass cls, jlong handle, jstring port_name) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        string name(env->GetStringUTFChars(port_name, NULL));
        return static_cast<jint>(port->set_port_name(name));
    }
    
    jstring Java_jm_lib_Serial_port_nativePortName(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return NULL;
        string name = port->port_name();
        return env->NewStringUTF(name.c_str());
    }
    
    jint Java_jm_lib_SerialPort_nativeSetBaudrate(JNIEnv *env, jclass cls, jlong handle, jint baudrate) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_baudrate(baudrate);
    }
    
    jint Java_jm_lib_SerialPort_nativeBaudrate(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->baudrate();
    }
    
    jint Java_jm_lib_SerialPort_nativeSetDatabits(JNIEnv *env, jclass cls, jlong handle, jbyte databits) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_databits(databits);
    }
    
    jint Java_jm_lib_SerialPort_nativeDatabits(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->databits();
    }
    
    jint Java_jm_lib_SerialPort_nativeSetParity(JNIEnv *env, jclass cls, jlong handle, jint parity) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_parity(parity);
    }
    
    jint Java_jm_lib_SerialPort_nativeParity(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->parity();
    }
    
    jint Java_jm_lib_SerialPort_nativeSetStopbits(JNIEnv *env, jclass cls, jlong handle, jint stopbits) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_stopbits(stopbits);
    }
    
    jint Java_jm_lib_SerialPort_nativeStopbits(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->stopbits();
    }
    
    jint Java_jm_lib_SerialPort_nativeSetFlowControl(JNIEnv *env, jclass cls, jlong handle, jint flow) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_flow_control(flow);
    }
    
    jint Java_jm_lib_SerialPort_nativeFlowControl(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->flow_control();
    }
    
    jint Java_jm_lib_SerialPort_nativeSetReadTimeout(JNIEnv *env, jclass cls, jlong handle, jlong millic_seconds) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_read_timeout(millic_seconds);
    }
    
    jint Java_jm_lib_SerialPort_nativeSetWriteTimeout(JNIEnv *env, jclass cls, jlong handle, jlong millic_seconds) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_write_timeout(millic_seconds);
    }
    
    jboolean Java_jm_lib_SerialPort_nativeIsOpen(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return JNI_FALSE;
        return port->is_open();
    }
    
    jint Java_jm_lib_SerialPort_nativeOpen(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->open();
    }
    
    jint Java_jm_lib_SerialPort_nativeClose(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->close();
    }
    
    jint Java_jm_lib_SerialPort_nativeFlush(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->flush();
    }
    
    jint Java_jm_lib_SerialPort_nativeDiscardInBuffer(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->discard_in_buffer();
    }
    
    jint Java_jm_lib_SerialPort_nativeDiscardOutBuffer(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->discard_out_buffer();
    }
    
    jlong Java_jm_lib_SerialPort_nativeBytesAvailable(JNIEnv *env, jclass cls, jlong handle) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->bytes_available();
    }
    
    jint Java_jm_lib_SerialPort_nativeSetDtr(JNIEnv *env, jclass cls, jlong handle, jboolean set) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_dtr(set);
    }
    
    jint Java_jm_lib_SerialPort_nativeSetRts(JNIEnv *env, jclass cls, jlong handle, jboolean set) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->set_rts(set);
    }
    
    jlong Java_jm_lib_SerialPort_nativeRead(JNIEnv *env, jclass cls, jlong handle, jbyteArray buff, jint offset, jint count) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        boost::scoped_array<uint8> temp(new uint8[count]);
        int64 ret = port->read(temp.get(), 0, count);
        if (ret == count) {
            env->SetByteArrayRegion(buff, offset, count, (const jbyte*)temp.get());
        }
        return ret;
    }
    
    jlong Java_jm_lib_SerialPort_nativeWrite(JNIEnv *env, jclass cls, jlong handle, jbyteArray buff, jint offset, jint count) {
        serial_port *port = (serial_port*)handle;
        if (port == 0)
            return -1;
        return port->write((const uint8*)env->GetByteArrayElements(buff, NULL), offset, count);
    }
    
    jobjectArray Java_jm_lib_SerialPort_getSystemPorts(JNIEnv *env, jclass cls) {
        std::vector<std::string> vec = serial_port::get_system_ports();
        jobjectArray ret = env->NewObjectArray(vec.size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));
        for (int i = 0; i < vec.size(); i++) {
            env->SetObjectArrayElement(ret, i, env->NewStringUTF(vec[i].c_str()));
        }
        return ret;
    }
}