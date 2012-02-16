#ifndef __JM_COMMBOX_PORT_H__
#define __JM_COMMBOX_PORT_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmerror.h>

G_BEGIN_DECLS

typedef enum _JMCommboxPortType JMCommboxPortType;

enum _JMCommboxPortType {
    JM_COMMBOX_PORT_SERIAL_PORT,
    JM_COMMBOX_PORT_UNKNOW
};

GLIB_VAR void jm_commbox_port_init(void);
GLIB_VAR void jm_commbox_port_destroy(void);
GLIB_VAR gint32 jm_commbox_port_set_read_timeout(gint64 microseconds);
GLIB_VAR gint32 jm_commbox_port_set_write_timeout(gint64 microseconds);
GLIB_VAR size_t jm_commbox_port_bytes_available(void);
GLIB_VAR gint32 jm_commbox_port_discard_out_buffer(void);
GLIB_VAR gint32 jm_commbox_port_discard_in_buffer(void);
GLIB_VAR size_t jm_commbox_port_read(guint8 *data, size_t count);
GLIB_VAR size_t jm_commbox_port_write(const guint8 *data, size_t count);

GLIB_VAR void jm_commbox_port_push_in_deque(const guint8 *data, size_t count);
GLIB_VAR gboolean jm_commbox_port_out_deque_available(void);
GLIB_VAR gboolean jm_commbox_port_pop_out_deque(GByteArray **data);

G_END_DECLS
#endif