#include <string.h>
#include <stdlib.h>
#include "jmv1iso14230.h"
#include "jmerror.h"
#include "jmtimer.h"
#include "jmv1default.h"

typedef struct _JMV1ISO14230 JMV1ISO14230;

struct _JMV1ISO14230
{
    JMV1Box *box;
    gboolean l_line;
    guint8 send_line;
    guint8 recv_line;
};

gint32 _jm_v1_iso14230_fast_init(JMKWP2000 *self, const guint8 *data, 
    size_t count)
{
    JMV1ISO14230 *iso14230 = NULL;
    JMV1Box *box = NULL;
    guint8 value_open = 0;
    guint8 pack_enter[256];
    size_t length;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    iso14230 = (JMV1ISO14230*)self->user_data;
    box = iso14230->box;

    if (iso14230->l_line)
    {
        value_open = JM_V1_BOX_C(box, PWC) | JM_V1_BOX_C(box, RZFC) | 
            JM_V1_BOX_C(box, CK);
    }
    else
    {
        value_open = JM_V1_BOX_C(box, PWC) | JM_V1_BOX_C(box, RZFC) | 
            JM_V1_BOX_C(box, CK);
    }

    if (!box->set_comm_ctrl(box, value_open, JM_V1_BOX_C(box, SET_NULL)) ||
        !box->set_comm_line(box, iso14230->send_line, iso14230->recv_line) ||
        !box->set_comm_link(box, JM_V1_BOX_C(box, RS_232) | JM_V1_BOX_C(box, BIT9_MARK) | JM_V1_BOX_C(box, SEL_SL) | JM_V1_BOX_C(box, UN_DB20), JM_V1_BOX_C(box, SET_NULL), JM_V1_BOX_C(box, SET_NULL)) ||
        !box->set_comm_baud(box, 10416) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETBYTETIME), JM_TIMER_TO_MS(5)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), JM_TIMER_TO_MS(0)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECBBOUT), JM_TIMER_TO_MS(400)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECFROUT), JM_TIMER_TO_MS(500)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETLINKTIME), JM_TIMER_TO_MS(500)))
    {
        return JM_ERROR_GENERIC;
    }

    g_usleep(JM_TIMER_TO_SEC(1));
    box->shared->buff_id = 0;

    if (!box->new_batch(box, box->shared->buff_id))
    {
        return JM_ERROR_GENERIC;
    }

    length = jm_kwp2000_pack(self, data, count, pack_enter);

    if (!box->set_line_level(box, JM_V1_BOX_C(box, COMS), JM_V1_BOX_C(box, SET_NULL)) ||
        !box->commbox_delay(box, JM_TIMER_TO_MS(25)) ||
        !box->set_line_level(box, JM_V1_BOX_C(box, SET_NULL), JM_V1_BOX_C(box, COMS)) ||
        !box->commbox_delay(box, JM_TIMER_TO_MS(25)) ||
        !box->send_out_data(box, pack_enter, length) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_FR)) ||
        !box->end_batch(box))
    {
        box->del_batch(box, box->shared->buff_id);
        return JM_ERROR_GENERIC;
    }

    if (!box->run_batch(box, &(box->shared->buff_id), 1, FALSE))
        return JM_ERROR_GENERIC;

    length = jm_v1_iso14230_read_one_frame(self, pack_enter);
    if (length <= 0)
        return JM_ERROR_GENERIC;

    box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), 
        JM_TIMER_TO_MS(55));
    return JM_ERROR_SUCCESS;
}

void _jm_v1_iso14230_finish_execute(JMKWP2000 *self, gboolean is_finish)
{
    if (is_finish)
    {
        JMV1ISO14230 *iso14230 = (JMV1ISO14230*)self->user_data;
        JMV1Box *box = iso14230->box;
        box->stop_now(box, TRUE);
        box->del_batch(box, box->shared->buff_id);
        box->check_result(box, JM_TIMER_TO_MS(500));
    }
}

gint32 _jm_v1_iso14230_addr_init(JMKWP2000 *self, guint8 addr_code)
{
    JMV1ISO14230 *iso14230 = NULL;
    JMV1Box *box = NULL;
    guint8 temp[3];

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    iso14230 = (JMV1ISO14230*)self->user_data;
    box = iso14230->box;

    if (!box->set_comm_ctrl(box, JM_V1_BOX_C(box, PWC) | JM_V1_BOX_C(box, REFC) |
        JM_V1_BOX_C(box, RZFC) | JM_V1_BOX_C(box, CK), JM_V1_BOX_C(box, SET_NULL)) ||
        !box->set_comm_link(box, JM_V1_BOX_C(box, RS_232) | JM_V1_BOX_C(box, BIT9_MARK) |
        JM_V1_BOX_C(box, SEL_SL) | JM_V1_BOX_C(box, SET_DB20), JM_V1_BOX_C(box, SET_NULL),
        JM_V1_BOX_C(box, INVERTBYTE)) ||
        !box->set_comm_baud(box, 5) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETBYTETIME), JM_TIMER_TO_MS(5)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), JM_TIMER_TO_MS(12)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECBBOUT), JM_TIMER_TO_MS(400)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECFROUT), JM_TIMER_TO_MS(500)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETLINKTIME), JM_TIMER_TO_MS(500)))
    {
        return JM_ERROR_GENERIC;
    }

    g_usleep(JM_TIMER_TO_SEC(1));

    box->shared->buff_id = 0;

    if (!box->new_batch(box, box->shared->buff_id))
    {
        return JM_ERROR_GENERIC;
    }

    if (!box->send_out_data(box, &addr_code, 1) ||
        !box->set_comm_line(box, (iso14230->recv_line == JM_V1_BOX_C(box, RK_NO)) ? iso14230->send_line : JM_V1_BOX_C(box, SK_NO), iso14230->recv_line) ||
        !box->run_receive(box, JM_V1_BOX_C(box, SET55_BAUD)) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
        !box->turn_over_one_by_one(box) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
        !box->turn_over_one_by_one(box) ||
        !box->run_receive(box, JM_V1_BOX_C(box, REC_LEN_1)) ||
        !box->end_batch(box))
    {
        box->del_batch(box, box->shared->buff_id);
        return JM_ERROR_GENERIC;
    }

    if (!box->run_batch(box, &(box->shared->buff_id), 1, FALSE) ||
        (box->read_data(box, temp, 0, JM_TIMER_TO_SEC(3)) <= 0) ||
        !box->check_result(box, JM_TIMER_TO_SEC(5)))
    {
        box->del_batch(box, box->shared->buff_id);
        return JM_ERROR_GENERIC;
    }

    if (!box->del_batch(box, box->shared->buff_id) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), 
        JM_TIMER_TO_MS(55)))
    {
        return JM_ERROR_GENERIC;
    }

    if (temp[2] != 0)
        return JM_ERROR_GENERIC;

    return JM_ERROR_SUCCESS;
}

gint32 _jm_v1_iso14230_set_lines(JMKWP2000 *self, gint32 com_line, 
    gboolean l_line)
{
    // According the connector to determine the send and receive line
    JMV1Box *box = NULL;
    JMV1ISO14230 *iso14230 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    iso14230 = (JMV1ISO14230*)self->user_data;
    box = iso14230->box;

    if (box->shared->connector == JM_CN_OBDII_16)
    {
        switch (com_line)
        {
        case 7:
            iso14230->send_line = JM_V1_BOX_C(box, SK1);
            iso14230->recv_line = JM_V1_BOX_C(box, RK1);
            break;
        default:
            return JM_ERROR_GENERIC;
        }
    }
    else
    {
        return JM_ERROR_GENERIC;
    }
    iso14230->l_line = l_line;
    return JM_ERROR_SUCCESS;
}

JMKWP2000* jm_v1_iso14230_new(JMV1Box *box)
{
    JMKWP2000 *obj = jm_kwp2000_new();
    JMV1ISO14230 *iso14230 = (JMV1ISO14230*)g_malloc(sizeof(JMV1ISO14230));
    obj->user_data = iso14230;
    obj->fast_init = _jm_v1_iso14230_fast_init;
    obj->addr_init = _jm_v1_iso14230_addr_init;
    obj->set_lines = _jm_v1_iso14230_set_lines;

    iso14230->box = box;
    iso14230->l_line = FALSE;
    iso14230->send_line = 0;
    iso14230->recv_line = 0;

    return obj;
}

void jm_v1_iso14230_free(JMKWP2000 *self)
{
    JMV1ISO14230 *iso14230 = NULL;
    g_return_if_fail(self != NULL);

    iso14230 = (JMV1ISO14230*)self->user_data;
    g_free(iso14230);

    jm_kwp2000_free(self);
}

static size_t _jm_v1_iso14230_read_one_frame(JMKWP2000 *self, guint8 *data, 
    gboolean is_finish)
{
    static guint8 temp[3];
    static guint8 result[256];
    size_t frame_length = 0;
    size_t length;
    JMV1ISO14230 *iso14230 = NULL;
    JMV1Box *box = NULL;
    guint8 checksum;
    size_t i;

    g_return_val_if_fail(self != NULL, 0);
    iso14230 = (JMV1ISO14230*)self->user_data;
    box = iso14230->box;

    length = box->read_bytes(box, temp, 3);

    if (length <= 0)
    {
        _jm_v1_iso14230_finish_execute(self, is_finish);
        return 0;
    }

    if (temp[1] == self->source_address)
    {
        if (temp[0] == 0x80)
        {
            guint8 b;
            length = box->read_bytes(box, &b, 1);
            if (length <= 0)
            {
                _jm_v1_iso14230_finish_execute(self, is_finish);
                return 0;
            }
            length = b;
            if (length <= 0)
            {
                _jm_v1_iso14230_finish_execute(self, is_finish);
                return 0;
            }
            memcpy(result, temp, 3);
            frame_length += 3;
            memcpy(result + frame_length, &b, 1);
            frame_length++;
            length = box->read_bytes(box, result + JM_KWP80_HEADER_LENGTH, 
                length + JM_KWP_CHECKSUM_LENGTH);
            frame_length += length;
        }
        else
        {
            length = temp[0] - 0x80;
            if (length <= 0)
            {
                return 0;
            }

            memcpy(result, temp, 3);
            frame_length += 3;
            length = box->read_bytes(box, result + 3, 
                length + JM_KWP_CHECKSUM_LENGTH);
            frame_length += length;
        }
    }
    else
    {
        if (temp[0] == 0x00)
        {
            length = temp[1];
            if (length <= 0)
            {
                _jm_v1_iso14230_finish_execute(self, is_finish);
                return 0;
            }
            memcpy(result, temp, 3);
            frame_length += 3;
            length = box->read_bytes(box, result + 3, length);
            frame_length += length;
        }
        else
        {
            length = temp[0];
            if (length <= 0)
            {
                _jm_v1_iso14230_finish_execute(self, is_finish);
                return 0;
            }
            memcpy(result, temp, 3);
            frame_length += 3;
            length = box->read_bytes(box, result + 3, 
                length - JM_KWP_CHECKSUM_LENGTH);
            frame_length += length;
        }
    }

    _jm_v1_iso14230_finish_execute(self, is_finish);
    if (frame_length <= 0)
    {
        return frame_length;
    }

    checksum = 0;
    for (i = 0; i < frame_length - 1; i++)
    {
        checksum += result[i];
    }

    if (checksum != result[frame_length - 1])
    {
        return 0;
    }

    return jm_kwp2000_unpack(self, result, frame_length, data);
}

static gint32 _jm_v1_iso14230_send_one_frame(JMKWP2000 *self, 
    const guint8 *data, size_t count, gboolean is_finish)
{
    JMV1ISO14230 *iso14230 = NULL;
    JMV1Box *box = NULL;
    g_return_val_if_fail(self != NULL, 0);

    iso14230 = (JMV1ISO14230*)self->user_data;
    box = iso14230->box;

    JM_V1_DEFAULT_SEND_ONE_FRAME(box->shared, box, self, data, count, 
        is_finish, iso14230);
}

size_t jm_v1_iso14230_send_one_frame(JMKWP2000 *self, const guint8 *data, 
    size_t count)
{
    return _jm_v1_iso14230_send_one_frame(self, data, count, FALSE);
}

size_t jm_v1_iso14230_send_frames(JMKWP2000 *self, const guint8 *data, 
    size_t count)
{
    return jm_v1_iso14230_send_one_frame(self, data, count);
}

size_t jm_v1_iso14230_read_one_frame(JMKWP2000 *self, guint8 *data)
{
    return _jm_v1_iso14230_read_one_frame(self, data, TRUE);
}

size_t jm_v1_iso14230_read_frames(JMKWP2000 *self, guint8 *data)
{
    return jm_v1_iso14230_read_one_frame(self, data);
}

gint32 jm_v1_iso14230_set_keep_link(JMKWP2000 *self, const guint8 *data, 
    size_t count)
{
    JMV1ISO14230 *iso14230 = NULL;
    JMV1Box *box = NULL;
    guint8 buff[256];
    size_t length;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    iso14230 = (JMV1ISO14230*)self->user_data;
    box = iso14230->box;
    self->mode = self->link_mode;
    length = jm_kwp2000_pack(self, data, count, buff);
    self->mode = self->msg_mode;
    JM_V1_DEFAULT_SET_KEEP_LINK(box, buff, length);
}
