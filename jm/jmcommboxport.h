#ifndef __JM_COMMBOX_PORT_H__
#define __JM_COMMBOX_PORT_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmerror.h>

G_BEGIN_DECLS

void jm_commbox_port_init(void);
void jm_commbox_port_destroy(void);
gint32 jm_commbox_port_set_read_timeout(gint64 microseconds);
gint32 jm_commbox_port_set_write_timeout(gint64 microseconds);
size_t jm_commbox_port_bytes_available(void);
gint32 jm_commbox_port_discard_out_buffer(void);
gint32 jm_commbox_port_discard_in_buffer(void);
size_t jm_commbox_port_read(guint8 *data, size_t count);
size_t jm_commbox_port_write(const guint8 *data, size_t count);

void jm_commbox_port_push_in_deque(const guint8 *data, size_t count);
gboolean jm_commbox_port_out_deque_available(void);
gboolean jm_commbox_port_pop_out_deque(GByteArray **data);

G_END_DECLS
#endif