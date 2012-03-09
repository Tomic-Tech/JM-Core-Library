#include <string.h>
#include <stdlib.h>
#include "jmv1iso15765.h"
#include "jmtimer.h"
#include "jmutils.h"
#include "jmv1default.h"
#include "jmerror.h"

static const guint8 JM_V1_ISO15765_SJA_BTR_CODETABLE[30] =
{
    0xBF, 0xFF, // 5KBPS
    0xEF, 0xFF, // 10KBPS
    0xD7, 0xFF, // 20KBPS
    0xCB, 0xFF, // 40KBPS
    0x47, 0x2F, // 50KBPS
    0xC5, 0xFF, // 80KBPS
    0xC9, 0xA7, // 100KBPS
    0x45, 0x2B, // 125KBPS
    0x04, 0xA7, // 200KBPS
    0x01, 0x1C, // 250KBPS
    0x02, 0x25, // 400KBPS
    0x01, 0x45, // 500KBPS
    0x80, 0xB6, // 666KBPS
    0x00, 0x16, // 800KBPS
    0x01, 0x12 // 1000KBPS
};

#define JM_V1_ISO15765_SET_CANBEGIN 0xF0
#define JM_V1_ISO15765_SET_CANBAUD 0xF1
#define JM_V1_ISO15765_SET_CANACR 0xF2
#define JM_V1_ISO15765_SET_CANAMR 0xF3
#define JM_V1_ISO15765_SET_CANMODE 0xF4
#define JM_V1_ISO15765_SET_CANCHAN 0xF5
#define JM_V1_ISO15765_SET_CANPRIOR 0xF6
#define JM_V1_ISO15765_SET_CANEND 0xFA
#define JM_V1_ISO15765_SET_CANRESET 0xFB

#define JM_V1_ISO15765_CAN_ACR_ACF1 0x20
#define JM_V1_ISO15765_CAN_ACR_ACF2 0x28
#define JM_V1_ISO15765_CAN_ACR_ACF3 0x30
#define JM_V1_ISO15765_CAN_ACR_ACF4 0x38

#define JM_V1_ISO15765_CAN_AMR_ACF1 0x24
#define JM_V1_ISO15765_CAN_AMR_ACF2 0x2C
#define JM_V1_ISO15765_CAN_AMR_ACF3 0x34
#define JM_V1_ISO15765_CAN_AMR_ACF4 0x3C

#define JM_V1_ISO15765_ACF1_FT1_PRIO 0x01
#define JM_V1_ISO15765_ACF1_FT2_PRIO 0x02
#define JM_V1_ISO15765_ACF2_FT1_PRIO 0x04
#define JM_V1_ISO15765_ACF2_FT2_PRIO 0x08
#define JM_V1_ISO15765_ACF3_FT1_PRIO 0x10
#define JM_V1_ISO15765_ACF3_FT2_PRIO 0x20
#define JM_V1_ISO15765_ACF4_FT1_PRIO 0x40
#define JM_V1_ISO15765_ACF4_FT2_PRIO 0x80

#define JM_V1_ISO15765_ACF1_FT1_CHAN 0x01
#define JM_V1_ISO15765_ACF1_FT2_CHAN 0x02
#define JM_V1_ISO15765_ACF2_FT1_CHAN 0x04
#define JM_V1_ISO15765_ACF2_FT2_CHAN 0x08
#define JM_V1_ISO15765_ACF3_FT1_CHAN 0x10
#define JM_V1_ISO15765_ACF3_FT2_CHAN 0x20
#define JM_V1_ISO15765_ACF4_FT1_CHAN 0x40
#define JM_V1_ISO15765_ACF4_FT2_CHAN 0x80

#define JM_V1_ISO15765_STD_FRAMEID_LENGTH 0x02
#define JM_V1_ISO15765_EXT_FRAMEID_LENGTH 0x04
#define JM_V1_ISO15765_SJA_OK 0

typedef struct _JMV1ISO15765 JMV1ISO15765;

struct _JMV1ISO15765
{
    JMV1Box *box;
};

void _jm_v1_iso15765_finish_execute(JMCanbus *self, gboolean is_finish)
{
    g_return_if_fail(self != NULL);

    if (is_finish)
    {
        JMV1Box * box = ((JMV1ISO15765*)self->user_data)->box;
        box->stop_now(box, TRUE);
        box->del_batch(box, box->shared->buff_id);
        box->check_result(box, JM_TIMER_TO_MS(200));
    }
}

gint32 _jm_v1_iso15765_set_lines(JMCanbus *self, gint32 high, gint32 low)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    self->high = high;
    self->low = low;

    return JM_ERROR_SUCCESS;
}

gint32 _jm_v1_iso15765_set_filter(JMCanbus *self, const gint32 *id_array, 
    size_t count)
{
    size_t i;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_ptr_array_set_size(self->id_array, 0);

    for (i = 0; i < count; i++) {
        g_ptr_array_add(self->id_array, GINT_TO_POINTER(id_array[i]));
    }
    return JM_ERROR_SUCCESS;
}

gint32 _jm_v1_iso15765_set_options(JMCanbus *self, 
    gint32 id, 
    JMCanbusBaud baud, 
    JMCanbusIDMode id_mode, 
    JMCanbusFilterMask mask, 
    JMCanbusFrameType frame)
{
    if (_jm_canbus_set_options(self, id, baud, id_mode, mask, frame))
    {
        if (self->id_mode == JM_CANBUS_ID_MODE_STD)
            self->target_id = (self->target_id << 5) & 0x0000FFFF;
        return JM_ERROR_SUCCESS;
    }
    return JM_ERROR_GENERIC;
}

static size_t _jm_v1_iso15765_send_cmd(JMCanbus *self, const guint8 *data, 
    size_t count)
{
    JMV1ISO15765 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;

    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1ISO15765*)self->user_data;
    box = v1->box;
    shared = box->shared;

    shared->buff_id = 0;
    if (!box->new_batch(box, shared->buff_id))
    {
        return 0;
    }

    if (!box->send_out_data(box, data, count) ||
        !box->run_receive(box, JM_V1_BOX_C(box, RECEIVE)) ||
        !box->end_batch(box) ||
        !box->run_batch(box, &shared->buff_id, 1, FALSE))
    {
        box->del_batch(box, shared->buff_id);
        return 0;
    }

    g_usleep(count * shared->req_byte_to_byte + shared->req_wait_time);
    return count;
}

static size_t _jm_v1_iso15765_read_cmd(JMCanbus *self, guint8 *buff)
{
    JMV1ISO15765 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;
    size_t length;

    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1ISO15765*)self->user_data;
    box = v1->box;
    shared = box->shared;

    if (box->read_bytes(box, buff, 3) != 3)
    {
        _jm_v1_iso15765_finish_execute(self, TRUE);
        return 0;
    }

    length = buff[0] & 0x0F;

    switch(buff[0] & 0xC0)
    {
    case (JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA):
    case (JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_REMOTE):
        length += JM_V1_ISO15765_STD_FRAMEID_LENGTH;
        break;
    case (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA):
    case (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE):
        length += JM_V1_ISO15765_EXT_FRAMEID_LENGTH;
        break;
    default:
        _jm_v1_iso15765_finish_execute(self, TRUE);
        return 0;
        break;
    }

    length -= 2;
    if (length > 0)
    {
        if (box->read_bytes(box, buff + 3, length) != length)
        {
            _jm_v1_iso15765_finish_execute(self, TRUE);
            return length + 3;
        }
        else
        {
            return 0;
        }
    }
    _jm_v1_iso15765_finish_execute(self, TRUE);
    return length + 3;
}

static gboolean _jm_v1_iso15765_do_cmd(JMCanbus *self, guint8 cmd, 
    guint8 inf)
{
    JMV1ISO15765 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;
    guint8 buff[256] = {0x20 | 2, 0x55, 0xAA, cmd, inf};
    size_t length;

    g_return_val_if_fail(self != NULL, FALSE);
    if (_jm_v1_iso15765_send_cmd(self, buff, 5) != 5)
    {
        return FALSE;
    }
    length = _jm_v1_iso15765_read_cmd(self, buff);

    if (length <= 0)
    {
        return FALSE;
    }

    if (buff[4] != JM_V1_ISO15765_SJA_OK)
    {
        return FALSE;
    }

    return TRUE;
}

#define _jm_v1_iso15765_begin_set(self) _jm_v1_iso15765_do_cmd(self, JM_V1_ISO15765_SET_CANBEGIN, 0)
#define _jm_v1_iso15765_set_mode(self, mode) _jm_v1_iso15765_do_cmd(self, JM_V1_ISO15765_SET_CANMODE, mode)
#define _jm_v1_iso15765_set_prior(self, prior) _jm_v1_iso15765_do_cmd(self, JM_V1_ISO15765_SET_CANPRIOR, prior)
#define _jm_v1_iso15765_set_chan(self, chan) _jm_v1_iso15765_do_cmd(self, JM_V1_ISO15765_SET_CANCHAN, chan)
#define _jm_v1_iso15765_end_set(self) _jm_v1_iso15765_do_cmd(self, JM_V1_ISO15765_SET_CANEND, 0)

static gboolean _jm_v1_iso15765_set_baud(JMCanbus *self, JMCanbusBaud baud)
{
    guint8 buff[256] = {0x20 | 5, 0x55, 0xAA, JM_V1_ISO15765_SET_CANBAUD};

    g_return_val_if_fail(self != NULL, FALSE);

    switch(baud)
    {
    case JM_CANBUS_B1000K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[28];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[29];
        break;
    case JM_CANBUS_B800K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[26];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[27];
        break;
    case JM_CANBUS_B666K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[24];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[25];
        break;
    case JM_CANBUS_B500K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[22];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[23];
        break;
    case JM_CANBUS_B400K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[20];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[21];
        break;
    case JM_CANBUS_B250K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[18];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[19];
        break;
    case JM_CANBUS_B200K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[16];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[17];
        break;
    case JM_CANBUS_B125K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[14];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[15];
        break;
    case JM_CANBUS_B100K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[12];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[13];
        break;
    case JM_CANBUS_B80K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[10];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[11];
        break;
    case JM_CANBUS_B50K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[8];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[9];
        break;
    case JM_CANBUS_B40K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[6];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[7];
        break;
    case JM_CANBUS_B20K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[4];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[5];
        break;
    case JM_CANBUS_B10K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[2];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[3];
        break;
    case JM_CANBUS_B5K:
        buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[0];
        buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[1];
        break;
    default:
        return FALSE;
        break;
    }

    if (_jm_v1_iso15765_send_cmd(self, buff, 6) != 6)
    {
        return FALSE;
    }

    if (!_jm_v1_iso15765_read_cmd(self, buff) <= 0)
    {
        return FALSE;
    }

    if (buff[4] != JM_V1_ISO15765_SJA_OK)
    {
        return FALSE;
    }

    return TRUE;
}

static gboolean _jm_v1_iso15765_set_acr(JMCanbus *self, guint8 acr, 
    guint8 acr0, guint8 acr1, guint8 acr2, guint8 acr3)
{
    guint8 buff[256] = {0x20 | 6, 0x55, 0xAA, JM_V1_ISO15765_SET_CANACR, 
        acr, acr0, acr1, acr2, acr3};

    g_return_val_if_fail(self != NULL, FALSE);

    if (_jm_v1_iso15765_send_cmd(self, buff, 9) != 9)
    {
        return FALSE;
    }

    if (_jm_v1_iso15765_read_cmd(self, buff) <= 0)
    {
        return FALSE;
    }

    if (buff[4] != JM_V1_ISO15765_SJA_OK)
    {
        return FALSE;
    }

    return TRUE;
}

static gboolean _jm_v1_iso15765_set_amr(JMCanbus *self, guint8 amr, 
    guint8 amr0, guint8 amr1, guint8 amr2, guint8 amr3)
{
    guint8 buff[256] = {0x20 | 6, 0x55, 0xAA, JM_V1_ISO15765_SET_CANAMR, 
        amr, amr0, amr1, amr2, amr3};

    g_return_val_if_fail(self != NULL, FALSE);

    if (_jm_v1_iso15765_send_cmd(self, buff, 9) != 9)
    {
        return FALSE;
    }

    if (_jm_v1_iso15765_read_cmd(self, buff) <= 0)
    {
        return FALSE;
    }

    if (buff[4] != JM_V1_ISO15765_SJA_OK)
    {
        return FALSE;
    }

    return TRUE;
}

static size_t _jm_v1_iso15765_read_one_frame(JMCanbus *self, guint8 *buff, 
    gboolean is_finish)
{
    static size_t length;
    static size_t mode;

    JMV1ISO15765 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;

    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1ISO15765*)self->user_data;
    box = v1->box;
    shared = box->shared;

    if (box->read_bytes(box, buff, 3) != 3)
    {
        _jm_v1_iso15765_finish_execute(self, TRUE);
        return 0;
    }

    length = buff[0] & 0x0F;
    mode = buff[0] & (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE);

    switch(mode)
    {
    case JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA:
    case JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_REMOTE:
        length += JM_V1_ISO15765_STD_FRAMEID_LENGTH;
        break;
    case JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA:
    case JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE:
        length += JM_V1_ISO15765_EXT_FRAMEID_LENGTH;
        break;
    default:
        _jm_v1_iso15765_finish_execute(self, TRUE);
        return 0;
        break;
    }

    length -= 2;
    if (length <= 0)
    {
        _jm_v1_iso15765_finish_execute(self, TRUE);
        return 0;
    }
    else
    {
        if (box->read_bytes(box, buff + 3, length) != length)
        {
            _jm_v1_iso15765_finish_execute(self, TRUE);
            return 0;
        }
        _jm_v1_iso15765_finish_execute(self, TRUE);
        return length + 3;
    }
    return 0;
}

static void _jm_v1_iso15765_free(gpointer user_data)
{
    g_free(user_data);
}

JMCanbus* jm_v1_iso15765_new(JMV1Box *box)
{
    JMCanbus *obj = jm_canbus_new();
    JMV1ISO15765 *v1 = (JMV1ISO15765*) g_malloc(sizeof(JMV1ISO15765));

    v1->box = box;

    obj->user_data = v1;
    obj->set_filter = _jm_v1_iso15765_set_filter;
    obj->set_lines = _jm_v1_iso15765_set_lines;
    obj->set_options = _jm_v1_iso15765_set_options;
    obj->free = _jm_v1_iso15765_free;
    return obj;
}

gboolean jm_v1_iso15765_prepare(JMCanbus *self)
{
    guint32 local_id = 0;
    JMV1ISO15765 *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;
    guint8 ctrl_word1;
    g_return_val_if_fail(self, FALSE);

    v1 = (JMV1ISO15765*)self->user_data;
    box = v1->box;
    shared = box->shared;

    ctrl_word1 = JM_V1_BOX_C(box, RS_232) | JM_V1_BOX_C(box, BIT9_MARK) | 
        JM_V1_BOX_C(box, SEL_SL) | JM_V1_BOX_C(box, UN_DB20);

    if (!box->set_comm_ctrl(box, JM_V1_BOX_C(box, RZFC), JM_V1_BOX_C(box, SET_NULL)) ||
        !box->set_comm_line(box, JM_V1_BOX_C(box, SK5), JM_V1_BOX_C(box, SK7)) ||
        !box->set_comm_link(box, ctrl_word1, JM_V1_BOX_C(box, SET_NULL), JM_V1_BOX_C(box, SET_NULL)) ||
        !box->set_comm_baud(box, 57600) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETBYTETIME), 500) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), JM_TIMER_TO_MS(55)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECBBOUT), JM_TIMER_TO_MS(100)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECFROUT), JM_TIMER_TO_MS(200)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETLINKTIME), JM_TIMER_TO_MS(500)))
    {
        return FALSE;
    }
    local_id = GPOINTER_TO_UINT(g_ptr_array_index(self->id_array, 0));

    g_usleep(JM_TIMER_TO_MS(100));
    if (!_jm_v1_iso15765_begin_set(self) ||
        !_jm_v1_iso15765_set_baud(self, JM_CANBUS_B500K) ||
        !_jm_v1_iso15765_set_acr(self, JM_V1_ISO15765_CAN_ACR_ACF1, JM_HIGH_BYTE(JM_HIGH_WORD(local_id << 21)), JM_LOW_BYTE(JM_HIGH_WORD(local_id << 21)), JM_HIGH_BYTE(JM_LOW_WORD(local_id << 21)), JM_LOW_BYTE(JM_LOW_WORD(local_id << 21))) ||
        !_jm_v1_iso15765_set_amr(self, JM_V1_ISO15765_CAN_AMR_ACF1, 0x00, 0x1F, 0xFF, 0xFF) ||
        !_jm_v1_iso15765_set_mode(self, 0x55) ||
        !_jm_v1_iso15765_set_prior(self, 0xFF) ||
        !_jm_v1_iso15765_set_chan(self, JM_V1_ISO15765_ACF1_FT1_CHAN) ||
        !_jm_v1_iso15765_end_set(self))
    {
        return FALSE;
    }
    return TRUE;
}

static size_t _jm_v1_iso15765_send_one_frame(JMCanbus *self, const 
    guint8 *data, size_t count, gboolean is_finish)
{
    JMV1ISO15765 *iso15765 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;

    g_return_val_if_fail(self != NULL, 0);
    iso15765 = (JMV1ISO15765*)self->user_data;
    box = iso15765->box;
    shared = box->shared;

    JM_V1_DEFAULT_SEND_ONE_FRAME(shared, box, self, data, count, is_finish, 
        iso15765);
}

size_t jm_v1_iso15765_send_one_frame(JMCanbus *self, const guint8 *data, 
    size_t count)
{
    return _jm_v1_iso15765_send_one_frame(self, data, count, FALSE);
}

size_t jm_v1_iso15765_send_frames(JMCanbus *self, const guint8 *data, 
    size_t count)
{
    g_return_val_if_fail(self != NULL, 0);

    if (count <= 0 || count > 0x0FFF)
    {
        return 0;
    }

    if (count < 8)
    {
        guint8 temp[8] = {0};

        temp[0] = JM_LOW_BYTE(count);
        memcpy(temp + 1, data, count);
        return _jm_v1_iso15765_send_one_frame(self, temp, count, TRUE);
    }
    else
    {
        size_t frame_count = (count + 1) / 7;
        size_t last_data = (count + 1) % 7;
        size_t pos = 0;
        guint8 seq = 0x21;
        size_t frame_index = 0;

        if (last_data != 0)
            frame_count++;

        for(; frame_index < frame_count; ++frame_index)
        {
            if (0 == frame_index)
            {
                guint8 temp[16];
                temp[0] = (0x10 | JM_HIGH_BYTE(JM_LOW_WORD(count)));
                temp[1] = JM_LOW_BYTE(JM_LOW_WORD(count));
                memcpy(temp + 2, data + pos, 6);
                pos += 6;
                if (_jm_v1_iso15765_send_one_frame(self, temp, 8, TRUE) != 
                    8)
                {
                    return 0;
                }
                if (jm_v1_iso15765_read_one_frame(self, temp) <= 0)
                    return 0;
            }
            else if (frame_index == (frame_count - 1))
            {
                guint8 temp[8] = {0};
                temp[0] = seq;

                memcpy(temp + 1, data + pos, count - pos);
                if (_jm_v1_iso15765_send_one_frame(self, temp, 
                    count - pos + 1, TRUE) != (count - pos + 1))
                    return 0;
                return count;
            }
            else
            {
                guint8 temp[8];
                temp[0] = seq;
                memcpy(temp + 1, data + pos, 7);
                pos += 7;
                if (_jm_v1_iso15765_send_one_frame(self, temp, 8, FALSE) != 
                    8)
                    return 0;

                if (seq == 0x2F)
                    seq = 0x20;
                else
                    seq++;
            }
        }
    }
    return 0;
}

size_t jm_v1_iso15765_read_one_frame(JMCanbus *self, guint8 *data)
{
    return _jm_v1_iso15765_read_one_frame(self, data, TRUE);
}

size_t jm_v1_iso15765_read_frames(JMCanbus *self, guint8 *data)
{
    JMV1ISO15765 *v1 = NULL;
    JMV1Box *box = NULL;
    guint8 first_frame[16];
    size_t length;

    g_return_val_if_fail(self != NULL, 0);
    v1 = (JMV1ISO15765*)self->user_data;
    box = v1->box;

    length = _jm_v1_iso15765_read_one_frame(self, first_frame, FALSE);

    if (first_frame[0] == 0x30)
    {
        _jm_v1_iso15765_finish_execute(self, TRUE);
        memcpy(data, first_frame, length);
        return length;
    }

    if ((first_frame[0] & 0x10) == 0x10)
    {
        // Multi-Frame
        static size_t user_data_count;
        static size_t rest_frame_count;
        static size_t data_length;
        static size_t rest_data_count;
        static size_t i;
        static size_t pos;
        static guint8 rest_frames[0xFFF];
        static guint8 unpacked_buff[0xFF];

        _jm_v1_iso15765_finish_execute(self, TRUE);

        if (_jm_v1_iso15765_send_one_frame(self, self->flow_control, 8, 
            TRUE) != 8)
        {
            return 0;
        }

        pos = 0;
        user_data_count = ((first_frame[0] & 0x0F) << 8) | 
            (first_frame[1] & 0xFF);
        rest_frame_count = ((user_data_count - 6) / 7) + 
            (((user_data_count - 6) % 7) ? 1 : 0);
        memcpy(data, first_frame + 2, length - 2);
        pos += length - 2;

        if (self->id_mode == JM_CANBUS_ID_MODE_STD)
        {
            data_length = JM_V1_ISO15765_STD_FRAMEID_LENGTH + 8;
        }
        else
        {
            data_length = JM_V1_ISO15765_EXT_FRAMEID_LENGTH + 8;
        }

        rest_data_count = data_length * rest_frame_count;

        if (box->read_bytes(box, rest_frames, rest_data_count) != 
            rest_data_count)
        {
            _jm_v1_iso15765_finish_execute(self, TRUE);
            return 0;
        }

        _jm_v1_iso15765_finish_execute(self, TRUE);
        for (i = 0; i < rest_frame_count; i++)
        {
            length = jm_canbus_unpack(self, rest_frames + 
                (i * data_length), data_length, unpacked_buff);
            memcpy(data + pos, unpacked_buff, length);
            pos += length;
        }
    }
    else
    {
        while (first_frame[1] == 0x7F && first_frame[3] == 0x78)
        {
            length = _jm_v1_iso15765_read_one_frame(self, first_frame, 
                FALSE);
        }
        _jm_v1_iso15765_finish_execute(self, TRUE);
        memcpy(data, first_frame + 1, length - 1);
        return length;
    }
    return 0;
}

gint32 jm_v1_iso15765_set_keep_link(JMCanbus *self, const guint8 *data, 
    size_t count)
{
    static guint8 buff[256];
    static size_t length;
    JMV1Box *box = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    box = ((JMV1ISO15765*)self->user_data)->box;

    length = jm_canbus_pack(self, data, count, buff);

    JM_V1_DEFAULT_SET_KEEP_LINK(box, buff, length);
}