#include "jmv1comm.h"
#include "jmv1iso14230.h"
#include "jmv1kwp1281.h"
#include "jmv1iso15765.h"
#include "jmv1mikuni.h"

typedef struct _JMV1Comm JMV1Comm;

struct _JMV1Comm
{
    JMV1Box *box;
};

JM_DEF_SEND(v1, JMKWP2000, iso14230, send_one_frame)
JM_DEF_SEND(v1, JMKWP2000, iso14230, send_frames)
JM_DEF_RECV(v1, JMKWP2000, iso14230, read_one_frame)
JM_DEF_RECV(v1, JMKWP2000, iso14230, read_frames)
JM_DEF_LINK(v1, JMKWP2000, iso14230)
JM_DEF_SEND(v1, JMKWP1281, kwp1281, send_one_frame)
JM_DEF_SEND(v1, JMKWP1281, kwp1281, send_frames)
JM_DEF_RECV(v1, JMKWP1281, kwp1281, read_one_frame)
JM_DEF_RECV(v1, JMKWP1281, kwp1281, read_frames)
JM_DEF_SEND_RECV(v1, JMKWP1281, kwp1281)
JM_DEF_LINK(v1, JMKWP1281, kwp1281)
JM_DEF_SEND(v1, JMCanbus, iso15765, send_one_frame)
JM_DEF_SEND(v1, JMCanbus, iso15765, send_frames)
JM_DEF_RECV(v1, JMCanbus, iso15765, read_one_frame)
JM_DEF_RECV(v1, JMCanbus, iso15765, read_frames)
JM_DEF_LINK(v1, JMCanbus, iso15765)
JM_DEF_SEND(v1, JMMikuni, mikuni, send_one_frame)
JM_DEF_SEND(v1, JMMikuni, mikuni, send_frames)
JM_DEF_RECV(v1, JMMikuni, mikuni, read_one_frame)
JM_DEF_RECV(v1, JMMikuni, mikuni, read_frames)
JM_DEF_LINK(v1, JMMikuni, mikuni)

static size_t _jm_v1_comm_send_and_recv(JMComm *self, const guint8 *send, 
    size_t count, guint8 *recv)
{
    guint32 times = 3;
    g_return_val_if_fail(self != NULL, 0);

    while (times--)
    {
        if (self->send_frames(self, send, count) > 0)
        {
            return self->read_frames(self, recv);
        }
    }
    return 0;
}

static gint32 _jm_v1_comm_set_timeout(JMComm *self, gint32 tx_b2b, 
    gint32 rx_b2b, gint32 tx_f2f, gint32 rx_f2f, gint32 total)
{
    return JM_ERROR_SUCCESS;
}

static gint32 _jm_v1_comm_start_keep_link(JMComm *self, gboolean run)
{
    JMV1Comm *v1 = NULL;
    JMV1Box *box = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Comm*)self->user_data;
    box = v1->box;
    if (!box->keep_link(box, run))
    {
        return JM_ERROR_GENERIC;
    }

    return JM_ERROR_SUCCESS;
}

static void _jm_v1_comm_free(gpointer user_data)
{
    JMV1Comm *v1 = (JMV1Comm*)user_data;
    g_free(v1);
}

JMComm* jm_v1_comm_new(JMProtocolType type, JMV1Box *box)
{
    JMComm *comm = NULL;
    JMV1Comm *v1 = NULL;
    switch(type)
    {
    case JM_PRC_ISO14230:
        comm = jm_comm_new();
        comm->free = _jm_v1_comm_free;

        v1 = (JMV1Comm*)g_malloc(sizeof(JMV1Comm));
        v1->box = box;

        comm->prc_free = jm_kwp2000_free;
        comm->prc = jm_v1_iso14230_new(box);
        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_GET_FUNC(v1, iso14230, send_one_frame);
        comm->send_frames = JM_GET_FUNC(v1, iso14230, send_frames);
        comm->read_one_frame = JM_GET_FUNC(v1, iso14230, read_one_frame);
        comm->read_frames = JM_GET_FUNC(v1, iso14230, read_frames);
        comm->send_and_recv = _jm_v1_comm_send_and_recv;
        comm->set_keep_link = JM_GET_FUNC(v1, iso14230, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    case JM_PRC_KWP1281:
        comm = jm_comm_new();
        comm->free = _jm_v1_comm_free;

        v1 = (JMV1Comm*)g_malloc(sizeof(JMV1Comm));
        v1->box = box;


        comm->prc = jm_v1_kwp1281_new(box);
        comm->prc_free = jm_kwp1281_free;
        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_GET_FUNC(v1, kwp1281, send_one_frame);
        comm->send_frames = JM_GET_FUNC(v1, kwp1281, send_frames);
        comm->read_one_frame = JM_GET_FUNC(v1, kwp1281, read_one_frame);
        comm->read_frames = JM_GET_FUNC(v1, kwp1281, read_frames);
        comm->send_and_recv = JM_GET_FUNC(v1, kwp1281, send_and_recv);
        comm->set_keep_link = JM_GET_FUNC(v1, kwp1281, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    case JM_PRC_ISO15765:
        comm = jm_comm_new();
        comm->free = _jm_v1_comm_free;

        v1 = (JMV1Comm*) g_malloc(sizeof(JMV1Comm));
        v1->box = box;
        comm->prc = jm_v1_iso15765_new(box);
        comm->prc_free = jm_canbus_free;

        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_GET_FUNC(v1, iso15765, send_one_frame);
        comm->send_frames = JM_GET_FUNC(v1, iso15765, send_one_frame);
        comm->read_one_frame = JM_GET_FUNC(v1, iso15765, read_one_frame);
        comm->read_frames = JM_GET_FUNC(v1, iso15765, read_frames);
        comm->send_and_recv = _jm_v1_comm_send_and_recv;
        comm->set_keep_link = JM_GET_FUNC(v1, iso15765, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    case JM_PRC_MIKUNI:
        comm = jm_comm_new();
        comm->free = _jm_v1_comm_free;

        v1 = (JMV1Comm*) g_malloc(sizeof(JMV1Comm));
        v1->box = box;

        comm->prc = jm_v1_mikuni_new(box);
        comm->prc_free = jm_mikuni_free;
        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_GET_FUNC(v1, mikuni, send_one_frame);
        comm->send_frames = JM_GET_FUNC(v1, mikuni, send_one_frame);
        comm->read_one_frame = JM_GET_FUNC(v1, mikuni, read_one_frame);
        comm->read_frames = JM_GET_FUNC(v1, mikuni, read_frames);
        comm->send_and_recv = _jm_v1_comm_send_and_recv;
        comm->set_keep_link = JM_GET_FUNC(v1, mikuni, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    default:
        break;
    }
    return comm;
}


