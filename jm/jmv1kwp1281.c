#include "jmv1kwp1281.h"
#include "jmerror.h"
#include "jmtimer.h"

typedef struct _JMV1KWP1281 JMV1KWP1281;

struct _JMV1KWP1281
{
    JMV1Box *box;
    guint8 buff_id_addr[2]; // 0 == buffID, 1 == buffAddr
    guint8 k_line;
    guint8 l_line;
};

void _jm_v1_kwp1281_finish_execute(JMKWP1281 *self, gboolean is_finish)
{
    JMV1KWP1281 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;
    g_return_if_fail(self != NULL);

    v1 = (JMV1KWP1281*)self->user_data;
    box = v1->box;
    shared = box->shared;

    if (is_finish)
    {
        box->del_batch(box, shared->buff_id);
        box->check_result(box, JM_TIMER_TO_MS(500));
    }
}

gint32 _jm_v1_kwp1281_addr_init(JMKWP1281 *self, guint8 addr_code)
{
    gboolean dl0;
    guint8 temp[256];
    size_t length;
    JMV1KWP1281 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;
    guint8 value_open;
    guint8 value_close;
    guint8 ctrl_word1;
    guint8 ctrl_word2;
    guint8 ctrl_word3;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    v1 = (JMV1KWP1281*)self->user_data;
    box = v1->box;
    shared = box->shared;

    shared->buff_id = 0;
    box->stop_now(box, TRUE);
    box->check_result(box, JM_TIMER_TO_MS(50));

    dl0 = (v1->l_line == JM_V1_BOX_C(box, SK_NO) && 
        v1->k_line == JM_V1_BOX_C(box, RK_NO))? TRUE : FALSE;
    value_open = JM_V1_BOX_C(box, PWC) | JM_V1_BOX_C(box, REFC) | 
        JM_V1_BOX_C(box, RZFC) | 
        (dl0? JM_V1_BOX_C(box, DLC0) : JM_V1_BOX_C(box, CK));
    value_close = JM_V1_BOX_C(box, SET_NULL);
    ctrl_word1 = JM_V1_BOX_C(box, RS_232) | JM_V1_BOX_C(box, BIT9_MARK) | 
        (dl0 ? JM_V1_BOX_C(box, SEL_DL0) : JM_V1_BOX_C(box, SEL_SL)) | 
        JM_V1_BOX_C(box, SET_DB20);
    ctrl_word2 = 0xFF;
    ctrl_word3 = JM_V1_BOX_C(box, INVERTBYTE) | 1;

    if (!box->set_comm_ctrl(box, value_open, value_close) ||
        !box->set_comm_line(box, v1->l_line, v1->k_line) ||
        !box->set_comm_link(box, ctrl_word1, ctrl_word2, ctrl_word3) ||
        !box->set_comm_baud(box, 5) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETBYTETIME), 0) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), JM_TIMER_TO_MS(25)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECBBOUT), JM_TIMER_TO_MS(610)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECFROUT), JM_TIMER_TO_MS(610)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETLINKTIME), JM_TIMER_TO_MS(710)))
    {
            return JM_ERROR_GENERIC;
    }

    g_usleep(JM_TIMER_TO_MS(1));

    if (!box->new_batch(box, shared->buff_id))
    {
        return JM_ERROR_GENERIC;
    }

    if (!box->send_out_data(box, &addr_code, 1) ||
        !box->set_comm_line(box, v1->k_line == JM_V1_BOX_C(box, RK1) ? JM_V1_BOX_C(box, SK_NO) : v1->l_line, v1->k_line) ||
        !box->run_receive(box, JM_V1_BOX_C(box, SET55_BAUD)) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
        !box->turn_over_one_by_one(box) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETBYTETIME), JM_TIMER_TO_MS(2)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), JM_TIMER_TO_MS(2)) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_FR)) ||
        !box->turn_over_one_by_one(box) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
        !box->end_batch(box))
    {
            box->del_batch(box, shared->buff_id);
            return JM_ERROR_GENERIC;
    }
    if (!box->run_batch(box, &shared->buff_id, 1,  FALSE))
    {
        box->del_batch(box, shared->buff_id);
        return JM_ERROR_GENERIC;
    }

    if (box->read_data(box, temp, 2, JM_TIMER_TO_MS(3500)) != 2)
    {
        box->del_batch(box, shared->buff_id);
        return JM_ERROR_GENERIC;
    }

    length = jm_v1_kwp1281_read_one_frame(self, temp);
    if (length <= 0)
    {
        return JM_ERROR_GENERIC;
    }

    if (!box->check_result(box, JM_TIMER_TO_MS(500)))
    {
        box->del_batch(box, shared->buff_id);
        return JM_ERROR_GENERIC;
    }

    if (!box->del_batch(box, shared->buff_id))
    {
        return JM_ERROR_GENERIC;
    }

    self->frame_counter = temp[0];

    return JM_ERROR_SUCCESS;
}

gint32 _jm_v1_kwp1281_set_lines(JMKWP1281 *self, gint32 com_line, 
    gboolean l_line)
{
    JMV1KWP1281 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    v1 = (JMV1KWP1281*)self->user_data;
    box = v1->box;
    shared = box->shared;

    if (shared->connector == JM_CN_AUDI_4 ||
        shared->connector == JM_CN_OBDII_16)
    {
            if (com_line != 0)
            {
                v1->k_line = JM_V1_BOX_C(box, RK1);
            }
            else
            {
                v1->k_line = JM_V1_BOX_C(box, RK_NO);
            }
            if (l_line)
            {
                v1->l_line = JM_V1_BOX_C(box, SK2);
            }
            else
            {
                v1->l_line = JM_V1_BOX_C(box, SK_NO);
            }
    }
    else
    {
        return JM_ERROR_GENERIC;
    }
    return JM_ERROR_SUCCESS;
}

static size_t _jm_v1_kwp1281_send_one_frame(JMKWP1281 *self, 
    const guint8 *data, size_t count, gboolean need_recv)
{
    JMV1KWP1281 *v1 = NULL;
    JMV1Shared *shared = NULL;
    JMV1Box *box = NULL;
    guint8 send_buff[256];
    size_t length;

    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1KWP1281*) self->user_data;
    box = v1->box;
    shared = box->shared;

    shared->buff_id = 0;

    if (!box->new_batch(box, shared->buff_id))
    {
        return 0;
    }

    length = jm_kwp1281_pack(self, data, count, send_buff);

    if (length <= 0)
    {
        return 0;
    }

    if (need_recv)
    {
        if (!box->turn_over_one_by_one(box) ||
            !box->send_out_data(box, send_buff, length) ||
            !box->update_buff(box, JM_V1_BOX_C(box, INC_DATA), v1->buff_id_addr) ||
            !box->turn_over_one_by_one(box) ||
            !box->run_receive(box, JM_V1_BOX_C(box, REC_FR)) ||
            !box->turn_over_one_by_one(box) ||
            !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
            !box->update_buff(box, JM_V1_BOX_C(box, INC_DATA), v1->buff_id_addr) ||
            !box->turn_over_one_by_one(box) ||
            !box->run_receive(box, JM_V1_BOX_C(box, REC_FR)) ||
            !box->turn_over_one_by_one(box) ||
            !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
            !box->end_batch(box) ||
            !box->run_batch(box, &shared->buff_id, 1, FALSE))
        {
                box->del_batch(box, shared->buff_id);
                return 0;
        }
        g_usleep((length * shared->req_byte_to_byte + 
            shared->req_wait_time) / 1000);
    }
    else
    {
        if (!box->turn_over_one_by_one(box) ||
            !box->send_out_data(box, send_buff, length) ||
            !box->update_buff(box, JM_V1_BOX_C(box, INC_DATA), v1->buff_id_addr) ||
            !box->turn_over_one_by_one(box) ||
            !box->run_receive(box, JM_V1_BOX_C(box, REC_FR)) ||
            !box->turn_over_one_by_one(box) ||
            !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
            !box->update_buff(box, JM_V1_BOX_C(box, INC_DATA), v1->buff_id_addr) ||
            !box->end_batch(box) ||
            !box->run_batch(box, &shared->buff_id, 1, FALSE))
        {
                box->del_batch(box, shared->buff_id);
                return 0;
        }
    }
    if (box->read_bytes(box, send_buff, length - 1) != (length - 1))
    {
        return 0;
    }
    return count;
}

static void _jm_v1_kwp1281_free(gpointer user_data)
{
    g_free(user_data);
}

JMKWP1281* jm_v1_kwp1281_new(JMV1Box *box)
{
    JMKWP1281 *obj = jm_kwp1281_new();
    JMV1KWP1281 *v1 = (JMV1KWP1281*)g_malloc(sizeof(JMV1KWP1281));

    obj->user_data = v1;
    obj->addr_init = _jm_v1_kwp1281_addr_init;
    obj->set_lines = _jm_v1_kwp1281_set_lines;

    v1->k_line = 0;
    v1->l_line = 0;
    v1->box = box;

    return obj;
}

size_t jm_v1_kwp1281_send_one_frame(JMKWP1281 *self, const guint8 *data, 
    size_t count)
{
    return _jm_v1_kwp1281_send_one_frame(self, data, count, FALSE);
}

size_t jm_v1_kwp1281_send_frames(JMKWP1281 *self, const guint8 *data, 
    size_t count)
{
    return jm_v1_kwp1281_send_one_frame(self, data, count);
}

static size_t _jm_v1_kwp1281_read_one_frame(JMKWP1281 *self, guint8 *data, 
    gboolean is_finish)
{
    size_t pos = 0;
    size_t len;
    guint8 temp[256];
    JMV1KWP1281 *v1 = NULL;
    JMV1Box *box = NULL;

    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1KWP1281*)self->user_data;
    box = v1->box;

    if (box->read_bytes(box, temp, 1) != 1)
    {
        return 0;
    }

    pos++;
    len = temp[0];

    if (box->read_bytes(box, temp + pos, len) != len)
    {
        return 0;
    }

    pos += len;
    _jm_v1_kwp1281_finish_execute(self, is_finish);

    return jm_kwp1281_unpack(self, temp, pos, data);
}

size_t jm_v1_kwp1281_read_one_frame(JMKWP1281 *self, guint8 *data)
{
    return _jm_v1_kwp1281_read_one_frame(self, data, TRUE);
}

size_t jm_v1_kwp1281_read_frames(JMKWP1281 *self, guint8 *data)
{
    return jm_v1_kwp1281_read_one_frame(self, data);
}

size_t jm_v1_kwp1281_send_and_recv(JMKWP1281 *self, const guint8 *send, 
    size_t count, guint8 *recv)
{
    size_t length = jm_v1_kwp1281_send_one_frame(self, send, count);
    if (length != count)
    {
        return 0;
    }
    return jm_v1_kwp1281_read_one_frame(self, recv);
}

gint32 jm_v1_kwp1281_set_keep_link(JMKWP1281 *self, const guint8 *data, 
    size_t count)
{
    JMV1KWP1281 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;
    guint8 add1 = 0;
    guint8 add2 = 0;
    guint8 temp_buff[3];

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1KWP1281*)self->user_data;
    box = v1->box;
    shared = box->shared;

    v1->buff_id_addr[0] = JM_V1_BOX_C(box, LINKBLOCK);
    if (box->new_batch(box, JM_V1_BOX_C(box, LINKBLOCK)))
    {
        return JM_ERROR_GENERIC;
    }

    if (!box->turn_over_one_by_one(box) ||
        !box->send_out_data(box, data, data[0]))
    {
            return JM_ERROR_GENERIC;
    }

    v1->buff_id_addr[1] = data[0] - 2;

    if (!box->turn_over_one_by_one(box) ||
        !box->send_out_data(box, data, 1) ||
        !((add1 = box->update_buff(box, JM_V1_BOX_C(box, INC_DATA), v1->buff_id_addr)) == 0) ||
        !box->turn_over_one_by_one(box) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_FR)) ||
        !box->turn_over_one_by_one(box) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
        !((add2 = box->update_buff(box, JM_V1_BOX_C(box, INC_DATA), v1->buff_id_addr)) == 0) ||
        !box->end_batch(box))
    {
            return JM_ERROR_GENERIC;
    }

    temp_buff[0] = JM_V1_BOX_C(box, LINKBLOCK);
    temp_buff[1] = add1;
    temp_buff[2] = v1->buff_id_addr[1];

    if (!box->get_abs_add(box, JM_V1_BOX_C(box, LINKBLOCK), temp_buff[2]))
    {
        return JM_ERROR_GENERIC;
    }

    if (!box->update_buff(box, JM_V1_BOX_C(box, UPDATE_1BYTE), temp_buff))
    {
        return JM_ERROR_GENERIC;
    }

    temp_buff[1] = add2;

    if (!box->update_buff(box, JM_V1_BOX_C(box, UPDATE_1BYTE), temp_buff))
    {
        return JM_ERROR_GENERIC;
    }

    return JM_ERROR_SUCCESS;
}