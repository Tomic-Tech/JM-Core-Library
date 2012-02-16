#ifndef __JM_SERIAL_PORT_H__
#define __JM_SERIAL_PORT_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmstringarray.h>

G_BEGIN_DECLS

typedef struct _JMSerialPort JMSerialPort;

#define JM_SP_PAR_NONE 0
#define JM_SP_PAR_ODD 1
#define JM_SP_PAR_EVEN 2
#define JM_SP_PAR_MARK 3 /* Windows Only */
#define JM_SP_PAR_SPACE 4

#define JM_SP_SB_ONE 0
#define JM_SP_SB_ONE_POINT_FIVE 1
#define JM_SP_SB_TWO 2

#define JM_SP_FC_NONE 0
#define JM_SP_FC_HARDWARE 1
#define JM_SP_FC_SOFTWARE 2

GLIB_VAR JMSerialPort *jm_serial_port_new(void);
GLIB_VAR JMSerialPort *jm_serial_port_new_with_name(const gchar* name);
GLIB_VAR void jm_serial_port_free(JMSerialPort *self);
GLIB_VAR const gchar* jm_serial_port_get_port_name(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_port_name(JMSerialPort *self, const gchar *name);
GLIB_VAR gint32 jm_serial_port_get_baudrate(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_baudrate(JMSerialPort *self, gint32 baudrate);
GLIB_VAR guint8 jm_serial_port_get_databits(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_databits(JMSerialPort *self, guint8 databits);
GLIB_VAR gint32 jm_serial_port_get_parity(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_parity(JMSerialPort *self, gint32 parity);
GLIB_VAR gint32 jm_serial_port_get_stopbits(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_stopbits(JMSerialPort *self, gint32 stopbits);
GLIB_VAR gint32 jm_serial_port_get_flow_control(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_flow_control(JMSerialPort *self, gint32 flow_control);
GLIB_VAR size_t jm_serial_port_write(JMSerialPort *self, const guint8 *data, size_t count);
GLIB_VAR size_t jm_serial_port_read(JMSerialPort *self, guint8 *data, size_t count);
GLIB_VAR gint64 jm_serial_port_get_read_timeout(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_read_timeout(JMSerialPort *self, gint64 millic);
GLIB_VAR gint64 jm_serial_port_get_write_timeout(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_write_timeout(JMSerialPort *self, gint64 millic);
GLIB_VAR gboolean jm_serial_port_is_open(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_open(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_close(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_flush(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_discard_in_buffer(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_discard_out_buffer(JMSerialPort *self);
GLIB_VAR size_t jm_serial_port_bytes_available(JMSerialPort *self);
GLIB_VAR gint32 jm_serial_port_set_dtr(JMSerialPort *self, gboolean set);
GLIB_VAR gint32 jm_serial_port_set_rts(JMSerialPort *self, gboolean set);
GLIB_VAR JMStringArray* jm_serial_port_get_system_ports(void);

G_END_DECLS


#endif