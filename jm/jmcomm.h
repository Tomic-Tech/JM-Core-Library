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

struct _JMComm
{
	gpointer (*get_protocol)(JMComm *);
	size_t (*send_one_frame)(JMComm *, const guint8 *data, size_t count);
	size_t (*send_frames)(JMComm *, const guint8 *data, size_t count);
	size_t (*read_one_frame)(JMComm *, guint8 *data);
	size_t (*read_frames)(JMComm *, guint8 *data);
	size_t (*send_and_recv)(JMComm *, const guint8 *send, size_t send_count, 
        guint8 *recv);
	gint32 (*start_keep_link)(JMComm *, gboolean run);
	gint32 (*set_keep_link)(JMComm *, const guint8 *data, size_t count);
	gint32 (*set_timeout)(JMComm *, gint32 tx_b2b, gint32 rx_b2b, 
        gint32 tx_f2f, gint32 rx_f2f, gint32 total);
    void (*prc_free)(gpointer);
    void (*free)(gpointer user_data);

    JMProtocolType prc_type;
    gpointer prc;
	gpointer user_data;
};

GLIB_VAR JMComm* jm_comm_new(void);
GLIB_VAR void jm_comm_free(JMComm *self);

GLIB_VAR gpointer jm_comm_get_protocol(JMComm *self);
GLIB_VAR size_t jm_comm_send_one_frame(JMComm *self, 
    const guint8 *data, size_t count);
GLIB_VAR size_t jm_comm_send_frames(JMComm *self, const guint8 *data, size_t count);
GLIB_VAR size_t jm_comm_read_one_frame(JMComm *self, guint8 *data);
GLIB_VAR size_t jm_comm_read_frames(JMComm *self, guint8 *data);
GLIB_VAR size_t jm_comm_send_and_recv(JMComm *self, const guint8 *send, 
    size_t send_count, guint8 *recv);
GLIB_VAR gint32 jm_comm_start_keep_link(JMComm *self, gboolean run);
GLIB_VAR gint32 jm_comm_set_keep_link(JMComm *self, const guint8 *data, size_t count);
GLIB_VAR gint32 jm_comm_set_timeout(JMComm *self, gint32 tx_b2b, 
    gint32 rx_b2b, gint32 tx_f2f, gint32 rx_f2f, gint32 total);


// BASE for JMKWP2000, JMCanbus ...
// PRC for iso14230, iso15765 ...
#define JM_DEF_SEND(VER, BASE, PRC, FUNCTION) \
    size_t _jm_##VER##_##PRC##_##FUNCTION(JMComm *self, const guint8 *data, size_t count) \
{ \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, 0); \
    prc = (BASE*)self->prc; \
    return jm_##VER##_##PRC##_##FUNCTION(prc, data, count); \
}

#define JM_DEF_RECV(VER, BASE, PRC, FUNCTION) \
    size_t _jm_##VER##_##PRC##_##FUNCTION(JMComm *self, guint8 *data) \
{ \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, 0);\
    prc = (BASE*)self->prc; \
    return jm_##VER##_##PRC##_##FUNCTION(prc, data); \
}

#define JM_DEF_SEND_RECV(VER, BASE, PRC) \
    size_t _jm_##VER##_##PRC##_send_and_recv(JMComm *self, const guint8 *send, size_t count, guint8 *recv) \
{ \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, 0); \
    prc = (BASE*)self->prc; \
    return jm_##VER##_##PRC##_send_and_recv(prc, send, count, recv); \
}

#define JM_DEF_LINK(VER, BASE, PRC) \
    gint32 _jm_##VER##_##PRC##_set_keep_link(JMComm *self, const guint8 *data, size_t count) \
{ \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC); \
    prc = (BASE*)self->prc; \
    return jm_##VER##_##PRC##_set_keep_link(prc, data, count); \
}

#define JM_GET_FUNC(VER, PRC, FUNCTION) \
    _jm_##VER##_##PRC##_##FUNCTION

G_END_DECLS

#endif