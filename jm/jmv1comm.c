#include "jmv1comm.h"
#include "jmv1iso14230.h"
#include "jmv1kwp1281.h"
#include "jmv1iso15765.h"
#include "jmv1mikuni.h"

// BASE for JMKWP2000, JMCanbus ...
// PRC for iso14230, iso15765 ...
#define JM_V1_DEF_SEND(BASE, PRC, FUNCTION) \
    size_t _jm_v1_##PRC##_##FUNCTION(JMComm *self, const guint8 *data, size_t count) \
{ \
    JMV1Comm *v1 = NULL; \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, 0); \
    v1 = (JMV1Comm*)self->user_data; \
    prc = (BASE*)v1->prc; \
    return jm_v1_##PRC##_##FUNCTION(prc, data, count); \
}

#define JM_V1_DEF_RECV(BASE, PRC, FUNCTION) \
    size_t _jm_v1_##PRC##_##FUNCTION(JMComm *self, guint8 *data) \
{ \
    JMV1Comm *v1 = NULL; \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, 0);\
    v1 = (JMV1Comm*)self->user_data; \
    prc = (BASE*)v1->prc; \
    return jm_v1_##PRC##_##FUNCTION(prc, data); \
}

#define JM_V1_DEF_SEND_RECV(BASE, PRC) \
    size_t _jm_v1_##PRC##_send_and_recv(JMComm *self, const guint8 *send, size_t count, guint8 *recv) \
{ \
    JMV1Comm *v1 = NULL; \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, 0); \
    v1 = (JMV1Comm*)self->user_data; \
    prc = (BASE*)v1->prc; \
    return jm_v1_##PRC##_send_and_recv(prc, send, count, recv); \
}

#define JM_V1_DEF_LINK(BASE, PRC) \
    gint32 _jm_v1_##PRC##_set_keep_link(JMComm *self, const guint8 *data, size_t count) \
{ \
    JMV1Comm *v1 = NULL; \
    BASE *prc = NULL; \
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC); \
    v1 = (JMV1Comm *)self->user_data; \
    prc = (BASE*)v1->prc; \
    return jm_v1_##PRC##_set_keep_link(prc, data, count); \
}

#define JM_V1_GET_FUNC(PRC, FUNCTION) \
    _jm_v1_##PRC##_##FUNCTION

#define JM_V1_MAKE_PRC(PRC, box) \
    (gpointer)jm_v1_##PRC##_new(box)

#define JM_V1_FREE_PRC(PRC) \
    jm_v1_##PRC##_free

typedef struct _JMV1Comm JMV1Comm;

struct _JMV1Comm
{
    JMV1Box *box;
    gpointer prc;
    void (*prc_free)(gpointer);
};

JM_V1_DEF_SEND(JMKWP2000, iso14230, send_one_frame)
JM_V1_DEF_SEND(JMKWP2000, iso14230, send_frames)
JM_V1_DEF_RECV(JMKWP2000, iso14230, read_one_frame)
JM_V1_DEF_RECV(JMKWP2000, iso14230, read_frames)
JM_V1_DEF_LINK(JMKWP2000, iso14230)
JM_V1_DEF_SEND(JMKWP1281, kwp1281, send_one_frame)
JM_V1_DEF_SEND(JMKWP1281, kwp1281, send_frames)
JM_V1_DEF_RECV(JMKWP1281, kwp1281, read_one_frame)
JM_V1_DEF_RECV(JMKWP1281, kwp1281, read_frames)
JM_V1_DEF_SEND_RECV(JMKWP1281, kwp1281)
JM_V1_DEF_LINK(JMKWP1281, kwp1281)
JM_V1_DEF_SEND(JMCanbus, iso15765, send_one_frame)
JM_V1_DEF_SEND(JMCanbus, iso15765, send_frames)
JM_V1_DEF_RECV(JMCanbus, iso15765, read_one_frame)
JM_V1_DEF_RECV(JMCanbus, iso15765, read_frames)
JM_V1_DEF_LINK(JMCanbus, iso15765)
JM_V1_DEF_SEND(JMMikuni, mikuni, send_one_frame)
JM_V1_DEF_SEND(JMMikuni, mikuni, send_frames)
JM_V1_DEF_RECV(JMMikuni, mikuni, read_one_frame)
JM_V1_DEF_RECV(JMMikuni, mikuni, read_frames)
JM_V1_DEF_LINK(JMMikuni, mikuni)

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

static gpointer _jm_v1_comm_get_protocol(JMComm *self)
{
    JMV1Comm *v1 = NULL;
    g_return_val_if_fail(self != NULL, NULL);

    v1 = (JMV1Comm*)self->user_data;
    return v1->prc;
}

JMComm* jm_v1_comm_new(JMProtocolType type, JMV1Box *box)
{
    JMComm *comm = NULL;
    JMV1Comm *v1 = NULL;
    switch(type)
    {
    case JM_PRC_ISO14230:
        comm = jm_comm_new();

        v1 = (JMV1Comm*)g_malloc(sizeof(JMV1Comm));
        v1->box = box;
        v1->prc = JM_V1_MAKE_PRC(iso14230, box);
        v1->prc_free = JM_V1_FREE_PRC(iso14230);

        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_V1_GET_FUNC(iso14230, send_one_frame);
        comm->send_frames = JM_V1_GET_FUNC(iso14230, send_frames);
        comm->read_one_frame = JM_V1_GET_FUNC(iso14230, read_one_frame);
        comm->read_frames = JM_V1_GET_FUNC(iso14230, read_frames);
        comm->send_and_recv = _jm_v1_comm_send_and_recv;
        comm->set_keep_link = JM_V1_GET_FUNC(iso14230, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    case JM_PRC_KWP1281:
        comm = jm_comm_new();

        v1 = (JMV1Comm*)g_malloc(sizeof(JMV1Comm));
        v1->box = box;
        v1->prc = JM_V1_MAKE_PRC(kwp1281, box);
        v1->prc_free = JM_V1_FREE_PRC(kwp1281);

        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_V1_GET_FUNC(kwp1281, send_one_frame);
        comm->send_frames = JM_V1_GET_FUNC(kwp1281, send_frames);
        comm->read_one_frame = JM_V1_GET_FUNC(kwp1281, read_one_frame);
        comm->read_frames = JM_V1_GET_FUNC(kwp1281, read_frames);
        comm->send_and_recv = JM_V1_GET_FUNC(kwp1281, send_and_recv);
        comm->set_keep_link = JM_V1_GET_FUNC(kwp1281, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    case JM_PRC_ISO15765:
        comm = jm_comm_new();

        v1 = (JMV1Comm*) g_malloc(sizeof(JMV1Comm));
        v1->box = box;
        v1->prc = JM_V1_MAKE_PRC(iso15765, box);
        v1->prc_free = JM_V1_FREE_PRC(iso15765);

        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_V1_GET_FUNC(iso15765, send_one_frame);
        comm->send_frames = JM_V1_GET_FUNC(iso15765, send_one_frame);
        comm->read_one_frame = JM_V1_GET_FUNC(iso15765, read_one_frame);
        comm->read_frames = JM_V1_GET_FUNC(iso15765, read_frames);
        comm->send_and_recv = _jm_v1_comm_send_and_recv;
        comm->set_keep_link = JM_V1_GET_FUNC(iso15765, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    case JM_PRC_MIKUNI:
        comm = jm_comm_new();

        v1 = (JMV1Comm*) g_malloc(sizeof(JMV1Comm));
        v1->box = box;
        v1->prc = JM_V1_MAKE_PRC(mikuni, box);
        v1->prc_free = JM_V1_FREE_PRC(mikuni);

        comm->user_data = v1;
        comm->prc_type = type;
        comm->send_one_frame = JM_V1_GET_FUNC(mikuni, send_one_frame);
        comm->send_frames = JM_V1_GET_FUNC(mikuni, send_one_frame);
        comm->read_one_frame = JM_V1_GET_FUNC(mikuni, read_one_frame);
        comm->read_frames = JM_V1_GET_FUNC(mikuni, read_frames);
        comm->send_and_recv = _jm_v1_comm_send_and_recv;
        comm->set_keep_link = JM_V1_GET_FUNC(mikuni, set_keep_link);
        comm->set_timeout = _jm_v1_comm_set_timeout;
        comm->start_keep_link = _jm_v1_comm_start_keep_link;
        break;
    default:
        break;
    }
    return comm;
}

void jm_v1_comm_free(JMComm *self)
{
    JMV1Comm *v1 = NULL;
    g_return_if_fail(self != NULL);

    v1 = (JMV1Comm*)self->user_data;
    v1->prc_free(v1->prc);
    g_free(v1);
    jm_comm_free(self);
}
