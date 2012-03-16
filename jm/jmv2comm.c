#include "jmv2comm.h"
#include "jmv2iso15765.h"

JM_DEF_SEND(v2, JMCanbus, iso15765, send_one_frame)
JM_DEF_SEND(v2, JMCanbus, iso15765, send_frames)
JM_DEF_RECV(v2, JMCanbus, iso15765, read_one_frame)
JM_DEF_RECV(v2, JMCanbus, iso15765, read_frames)
JM_DEF_LINK(v2, JMCanbus, iso15765)


static size_t _jm_v2_comm_send_and_recv(JMComm *self, const guint8 *send, 
    size_t count, guint8 *recv)
{
    // suspend_keep_link();
    size_t length;
    length = self->send_frames(self, send, count);
    if (length == 0)
        return length;

    length = self->read_frames(self, recv);
    // restart_keep_link();
    return length;
}

static gint32 _jm_v2_comm_set_timeout(JMComm *self, gint32 tx_b2b, 
    gint32 rx_b2b, gint32 tx_f2f, gint32 rx_f2f, gint32 total)
{
    return JM_ERROR_SUCCESS;
}

static gint32 _jm_v2_comm_start_keep_link(JMComm *self, gboolean run)
{
    //JMV1Comm *v1 = NULL;
    //JMV1Box *box = NULL;
    //g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    //v1 = (JMV1Comm*)self->user_data;
    //box = v1->box;
    //if (!box->keep_link(box, run))
    //{
    //    return JM_ERROR_GENERIC;
    //}

    return JM_ERROR_SUCCESS;
}

JMComm* jm_v2_comm_new(JMProtocolType type)
{
    JMComm *comm = NULL;
    switch(type)
    {
    case JM_PRC_ISO15765:
        comm = jm_comm_new();
        
        comm->prc = jm_v2_iso15765_new;
        comm->prc = jm_canbus_free;
        comm->prc_type = type;
        comm->send_one_frame = JM_GET_FUNC(v2, iso15765, send_one_frame);
        comm->send_frames = JM_GET_FUNC(v2, iso15765, send_one_frame);
        comm->read_one_frame = JM_GET_FUNC(v2, iso15765, read_one_frame);
        comm->read_frames = JM_GET_FUNC(v2, iso15765, read_frames);
        comm->send_and_recv = _jm_v2_comm_send_and_recv;
        comm->set_keep_link = JM_GET_FUNC(v2, iso15765, set_keep_link);
        comm->set_timeout = _jm_v2_comm_set_timeout;
        comm->start_keep_link = _jm_v2_comm_start_keep_link;
        break;
    default:
        break;
    }
    return comm;
}

