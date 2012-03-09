#include "jmcomm.h"

static gint32 _jm_comm_set_keep_link_array(JMComm *self, 
    const GByteArray *data)
{
    g_return_val_if_fail(data != NULL, JM_ERROR_GENERIC);
    return self->set_keep_link(self, data->data, data->len);
}

JMComm* jm_comm_new(void)
{
    JMComm *obj = (JMComm*)g_malloc(sizeof(JMComm));
    obj->get_protocol = NULL;
    obj->send_one_frame = NULL;
    obj->send_frames = NULL;
    obj->read_one_frame = NULL;
    obj->read_frames = NULL;
    obj->send_and_recv = NULL;
    obj->start_keep_link = NULL;
    obj->set_keep_link = NULL;
    obj->set_timeout = NULL;
    obj->free = NULL;
    obj->prc_type = JM_PRC_UNKNOW;
    obj->user_data = NULL;
    return obj;
}

void jm_comm_free(JMComm *self)
{
    g_return_if_fail(self != NULL);
    if (self->free != NULL)
        self->free(self->user_data);

    g_free(self);
}

gpointer jm_comm_get_protocol(JMComm *self)
{
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(self->get_protocol != NULL, NULL);
    return self->get_protocol(self);
}

size_t jm_comm_send_one_frame(JMComm *self, const guint8 *data, 
    size_t count)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->send_one_frame != NULL, 0);
    return self->send_one_frame(self, data, count);
}

size_t jm_comm_send_frames(JMComm *self, const guint8 *data, size_t count)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->send_frames != NULL, 0);
    return self->send_frames(self, data, count);
}

size_t jm_comm_read_one_frame(JMComm *self, guint8 *data)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->read_one_frame != NULL, 0);
    return self->read_one_frame(self, data);
}

size_t jm_comm_read_frames(JMComm *self, guint8 *data)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->read_frames != NULL, 0);
    return self->read_frames(self, data);
}

size_t jm_comm_send_and_recv(JMComm *self, const guint8 *send, 
    size_t send_count, guint8 *recv)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->send_and_recv != NULL, 0);
    return self->send_and_recv(self, send, send_count, recv);
}

gint32 jm_comm_start_keep_link(JMComm *self, gboolean run)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->start_keep_link != NULL, JM_ERROR_GENERIC);
    return self->start_keep_link(self, run);
}

gint32 jm_comm_set_keep_link(JMComm *self, const guint8 *data, 
    size_t count)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_keep_link != NULL, JM_ERROR_GENERIC);
    return self->set_keep_link(self, data, count);
}

gint32 jm_comm_set_timeout(JMComm *self, gint32 tx_b2b, gint32 rx_b2b, 
    gint32 tx_f2f, gint32 rx_f2f, gint32 total)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_timeout != NULL, JM_ERROR_GENERIC);
    return self->set_timeout(self, tx_b2b, rx_b2b, tx_f2f, rx_f2f, total);
}