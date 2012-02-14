#ifndef __JM_COMM_H__
#define __JM_COMM_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmerror.h>
#include <jm/jmprotocoltype.h>

G_BEGIN_DECLS

typedef struct _JMComm JMComm;

struct _JMComm {
	gpointer (*get_protocol)(JMComm *);
	size_t (*send_one_frame)(JMComm *, const guint8 *data, size_t count);
	size_t (*send_frames)(JMComm *, const guint8 *data, size_t count);
	size_t (*read_one_frame)(JMComm *, guint8 *data);
	size_t (*read_frames)(JMComm *, guint8 *data);
	size_t (*send_and_recv)(JMComm *, const guint8 *send, size_t send_count, guint8 *recv);
	gint32 (*start_keep_link)(JMComm *, gboolean run);
	gint32 (*set_keep_link)(JMComm *, const guint8 *data, size_t count);
	gint32 (*set_timeout)(JMComm *, gint32 tx_b2b, gint32 rx_b2b, gint32 tx_f2f, gint32 rx_f2f, gint32 total);

    JMProtocolType prc_type;
	gpointer user_data;
};

JMComm* jm_comm_new(void);
void jm_comm_free(JMComm *self);

gpointer jm_comm_get_protocol(JMComm *self);
size_t jm_comm_send_one_frame(JMComm *self, const guint8 *data, size_t count);
size_t jm_comm_send_frames(JMComm *self, const guint8 *data, size_t count);
size_t jm_comm_read_one_frame(JMComm *self, guint8 *data);
size_t jm_comm_read_frames(JMComm *self, guint8 *data);
size_t jm_comm_send_and_recv(JMComm *self, const guint8 *send, size_t send_count, guint8 *recv);
gint32 jm_comm_start_keep_link(JMComm *self, gboolean run);
gint32 jm_comm_set_keep_link(JMComm *self, const guint8 *data, size_t count);
gint32 jm_comm_set_timeout(JMComm *self, gint32 tx_b2b, gint32 rx_b2b, gint32 tx_f2f, gint32 rx_f2f, gint32 total);

G_END_DECLS

#endif