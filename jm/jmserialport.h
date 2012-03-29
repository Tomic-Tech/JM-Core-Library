#ifndef __JM_SERIAL_PORT_H__
#define __JM_SERIAL_PORT_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmstringarray.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _JMSerialPort JMSerialPort;

struct _JMSerialPort
{
    pointer_t _handle;
};

JMSerialPort *jm_serial_port_new(void);
JMSerialPort *jm_serial_port_new_with_name(const char* name);
void jm_serial_port_free(JMSerialPort *self);
const char* jm_serial_port_get_port_name(JMSerialPort *self);
int32_t jm_serial_port_set_port_name(JMSerialPort *self, const char *name);
int32_t jm_serial_port_get_baudrate(JMSerialPort *self);
int32_t jm_serial_port_set_baudrate(JMSerialPort *self, int32_t baudrate);
uint8_t jm_serial_port_get_databits(JMSerialPort *self);
int32_t jm_serial_port_set_databits(JMSerialPort *self, uint8_t databits);
int32_t jm_serial_port_get_parity(JMSerialPort *self);
int32_t jm_serial_port_set_parity(JMSerialPort *self, int32_t parity);
int32_t jm_serial_port_get_stopbits(JMSerialPort *self);
int32_t jm_serial_port_set_stopbits(JMSerialPort *self, int32_t stopbits);
int32_t jm_serial_port_get_flow_control(JMSerialPort *self);
int32_t jm_serial_port_set_flow_control(JMSerialPort *self, int32_t flow_control);
size_t jm_serial_port_write(JMSerialPort *self, const uint8_t *data, size_t count);
size_t jm_serial_port_read(JMSerialPort *self, uint8_t *data, size_t count);
int64_t jm_serial_port_get_read_timeout(JMSerialPort *self);
int32_t jm_serial_port_set_read_timeout(JMSerialPort *self, int64_t microseconds);
int64_t jm_serial_port_get_write_timeout(JMSerialPort *self);
int32_t jm_serial_port_set_write_timeout(JMSerialPort *self, int64_t microseconds);
boolean_t jm_serial_port_is_open(JMSerialPort *self);
int32_t jm_serial_port_open(JMSerialPort *self);
int32_t jm_serial_port_close(JMSerialPort *self);
int32_t jm_serial_port_flush(JMSerialPort *self);
int32_t jm_serial_port_discard_in_buffer(JMSerialPort *self);
int32_t jm_serial_port_discard_out_buffer(JMSerialPort *self);
size_t jm_serial_port_bytes_available(JMSerialPort *self);
int32_t jm_serial_port_set_dtr(JMSerialPort *self, boolean_t set);
int32_t jm_serial_port_set_rts(JMSerialPort *self, boolean_t set);
JMStringArray* jm_serial_port_get_system_ports(void);

#ifdef __cplusplus
}
#endif

#endif