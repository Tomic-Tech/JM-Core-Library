#include "jmv2iso15765.h"
#include "jmerror.h"
#include "jmv2boxoption.h"
#include "jmv2default.h"
#include "jmtimer.h"
#include "jmutils.h"
#include "jmerror.h"

JMCanbus* jm_v2_iso15765_new(void)
{
    JMCanbus *obj = jm_canbus_new();
}

gboolean jm_v2_iso15765_prepare(JMCanbus *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    if (!jm_v2_box_set_can_mode(self->high, self->low, self->baud) ||
        !jm_v2_box_set_can_afmr(self->id_mode, self->filter_mask, 
        self->id_array->len, (guint32*)self->id_array->pdata))
    {
        return FALSE;
    }
    return TRUE;
}

static size_t jm_v2_iso15765_send_one_frame_normal(JMCanbus *self, 
    const guint8 *data, size_t count)
{
    jm_v2_default_send_one_frame(self, JM_V2_BOX_CAN_SEND, data, count)
}

static size_t jm_v2_iso15765_send_one_frame_multi(JMCanbus *self,
    const guint8 *data, size_t count)
{
    jm_v2_default_send_one_frame(self, JM_V2_BOX_CAN_SEND_MULTI, data, count)
}

size_t jm_v2_iso15765_send_one_frame(JMCanbus *self, const guint8 *data, 
    size_t count)
{
    return jm_v2_iso15765_send_one_frame_normal(self, data, count);
}

size_t jm_v2_iso15765_send_frames(JMCanbus *self, const guint8 *data, 
    size_t count)
{
    guint8 temp_buff[8];
    size_t frame_count = 0;
    size_t last_data = 0;
    guint8 seq = 0x21;
    size_t pos = 0;
    size_t frame_index = 0;

    if ((count == 0) || (count > 0x0FFFF))
    {
        return 0;
    }

    if (count <= 7)
    {
        memset(temp_buff, 0, 8);
        temp_buff[0] = (guint8)count;
        memcpy(temp_buff + 1, data, count);
        return jm_v2_iso15765_send_one_frame_normal(self, temp_buff, 8);
    }
    else
    {
        frame_count = (count + 1) / 7;
        last_data = (count + 1) % 7;
        pos = 0;
        seq = 0x21;
        
        if (last_data != 0)
            frame_count++;

        for (frame_index = 0; frame_index < frame_count; ++frame_index)
        {
            g_usleep(100);
            memset(temp_buff, 0, 8);
            if (0 == frame_index)
            {
                guint16 len = (guint16)count;
                temp_buff[0] = (guint8)(0x10 | JM_HIGH_BYTE(len));
                temp_buff[1] = (guint8)(JM_LOW_BYTE(len));
                memcpy(temp_buff + 2, data + pos, 6);
                jm_v2_iso15765_send_one_frame_normal(self, temp_buff, 8);
                jm_v2_iso15765_read_one_frame(self, temp_buff);
                pos += 6;
            }
            else if (frame_index == (frame_count - 1))
            {
                memset(temp_buff, 0, 8);
                temp_buff[0] = seq;
                memcpy(temp_buff + 1, data + pos, count - pos);
                jm_v2_iso15765_send_one_frame_normal(self, temp_buff, 8);
                pos += count - pos;
                return count;
            }
            else
            {
                memset(temp_buff, 0, 8);
                temp_buff[0] = seq;
                memcpy(temp_buff + 1, data + pos, 7);
                jm_v2_iso15765_send_one_frame_multi(self, temp_buff, 8);
                pos += 7;
                if (seq == 0x2F)
                    seq = 0x20;
                else
                    seq++;
            }
        }
    }
    return 0;
}

size_t jm_v2_iso15765_read_one_frame(JMCanbus *self, guint8 *data)
{
    jm_v2_default_read_one_frame(self, data, JM_TIMER_TO_SEC(2))
}

size_t jm_v2_iso15765_read_frames(JMCanbus *self, guint8 *data)
{
    size_t frame_count = 0;
    size_t last_data = 0;
    size_t data_count = 0;
    size_t recv_buff_index = 0;
    size_t length = 0;
    size_t length2 = 0;
    guint8 temp_buff[JM_V2_BOX_MAX_DATA_LENGTH];
    while ((length = jm_v2_iso15765_read_one_frame(self, temp_buff)) > 0)
    {
        if (temp_buff[0] == 0x30)
        {
            memcpy(data, temp_buff, length);
            return length;
        }

        if ((temp_buff[0] & 0x10) == 0x10)
        {
            jm_v2_iso15765_send_one_frame_normal(self, self->flow_control, 8);
            data_count = ((temp_buff[0] & 0x0F) << 8) | temp_buff[1];
            length2 = data_count;
            recv_buff_index = 0;
            memcpy(data + recv_buff_index, temp_buff + 2, 6);
            recv_buff_index += 6;
            data_count = data_count - 6;
            frame_count = data_count / 7;
            last_data = data_count % 7;
            if (last_data != 0)
                frame_count++;

            while ((length = jm_v2_iso15765_read_one_frame(self, temp_buff)) > 0 )
            {
                if (frame_count > 1)
                {
                    memcpy(data + recv_buff_index, temp_buff + 1, 7);
                    recv_buff_index += 7;
                    frame_count--;
                    data_count -= 7;
                    continue;
                }
                else
                {
                    memcpy(data + recv_buff_index, temp_buff + 1, data_count);
                    return length2;
                }
                return length2 - data_count;
            }
        }
        else
        {
            if (temp_buff[1] == 0x7F)
            {
                if (temp_buff[3] == 0x78)
                {
                    continue;
                }
            }
            data_count = temp_buff[0];
            length2 = data_count;
            memcpy(data, temp_buff + 1, data_count);
            return length2;
        }
    }
    return length2;
}
gint32 jm_v2_iso15765_set_keep_link(JMCanbus *self, const guint8 *data, 
    size_t count)
{
    return JM_ERROR_SUCCESS;
}