#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "jmv1boxw80.h"
#include "jmv1boxw80d.h"
#include "jmserialport.h"
#include "jmcommboxport.h"
#include "jmtimer.h"
#include "jmutils.h"

struct _JMV1BoxW80
{
    guint32 box_time_unit;
    guint32 time_base_db;
    guint32 time_extern_db;
    guint32 pos;
    gboolean is_link;
    guint32 run_flag;
    guint32 box_ver;
    guint32 start_pos;
    gboolean is_initialize;
    guint8 ports[JM_V1_BOX_W80_MAXPORT_NUM];
    guint8 buf[JM_V1_BOX_W80_MAXBUFF_LEN];
};

static gboolean _jm_v1_box_w80_check_idle(JMV1Box *self)
{
    guint8 rb = JM_V1_BOX_W80_READY;
    gint32 avail;

    g_return_val_if_fail(self != NULL, FALSE);

    avail = jm_commbox_port_bytes_available();

    if (avail > 20)
    {
        jm_commbox_port_discard_in_buffer();
        return TRUE;
    }
    while (jm_commbox_port_bytes_available())
    {
        jm_commbox_port_read(&rb, 1);
    }

    if (rb == JM_V1_BOX_W80_READY || rb == JM_V1_BOX_W80_ERROR)
    {
        return TRUE;
    }

    if (jm_commbox_port_read(&rb, 1) != 1)
    {
        return FALSE;
    }

    if (rb == JM_V1_BOX_W80_READY || rb == JM_V1_BOX_W80_ERROR)
    {
        return TRUE;
    }

    return FALSE;
}

static gboolean _jm_v1_box_w80_check_send(JMV1Box *self)
{
    guint8 rb = 0;
    g_return_val_if_fail(self != NULL, FALSE);
    if (jm_commbox_port_read(&rb, 1) != 1)
    {
        return FALSE;
    }

    if (rb == JM_V1_BOX_W80_RECV_OK)
    {
        return TRUE;
    }

    return FALSE;
}

static gboolean _jm_v1_box_w80_check_result(JMV1Box *self, 
    gint64 microseconds)
{
    guint8 rb = 0;
    g_return_val_if_fail(self != NULL, FALSE);

    if (jm_commbox_port_read(&rb, 1) != 1)
    {
        return FALSE;
    }

    if (rb == JM_V1_BOX_W80_READY || rb == JM_V1_BOX_W80_ERROR)
    {
        jm_commbox_port_discard_in_buffer();
        return TRUE;
    }

    return FALSE;
}

static gboolean _jm_v1_box_w80_send_cmd(JMV1Box *self, guint8 cmd, 
    size_t count, const guint8 *data)
{
    JMV1BoxW80 *w80 = NULL;
    GByteArray *command = NULL;
    size_t i;
    guint8 cs = cmd;

    g_return_val_if_fail(self != NULL, FALSE);
    w80 = (JMV1BoxW80*)(self->user_data);

    cmd += w80->run_flag;

    command = g_byte_array_new();
    g_byte_array_append(command, &cmd, 1);

    g_byte_array_append(command, data, count);

    for (i = 0; i < count; i++)
    {
        cs += data[i];
    }

    g_byte_array_append(command, &cs, 1);

    for (i = 0; i < 3; i++)
    {
        if (!_jm_v1_box_w80_check_idle(self))
        {
            continue;
        }

        if (jm_commbox_port_write(command->data, command->len) != 
            command->len)
        {
            continue;
        }

        if (_jm_v1_box_w80_check_send(self))
        {
            return TRUE;
        }
    }
    return FALSE;
}

static size_t _jm_v1_box_w80_read_data(JMV1Box *self, guint8 *buff, 
    size_t count, gint64 microseconds)
{
    size_t avail = 0;
    size_t len = 0;
    g_return_val_if_fail(self != NULL, 0);
    jm_commbox_port_set_read_timeout(microseconds);
    len = jm_commbox_port_read(buff, count);
    if (len < count)
    {
        avail = jm_commbox_port_bytes_available();
        if (avail > 0)
        {
            if (avail <= (count - len))
            {
                len += jm_commbox_port_read(buff + len, avail);
            }
            else
            {
                len += jm_commbox_port_read(buff + len, count - len);
            }
        }
    }
    return len;
}

static size_t _jm_v1_box_w80_read_bytes(JMV1Box *self, guint8 *buff, 
    size_t count)
{
    return _jm_v1_box_w80_read_data(self, buff, count, JM_TIMER_TO_MS(500));
}

static guint8 _jm_v1_box_w80_get_cmd_data(JMV1Box *self, 
    guint8 *receive_buffer, size_t count)
{
    guint8 len = 0;
    guint8 cs = 0;
    g_return_val_if_fail(self != NULL, 0);

    if ((_jm_v1_box_w80_read_bytes(self, receive_buffer, 1) != 1)
        || (_jm_v1_box_w80_read_bytes(self, &len, 1) != 1))
    {
        return 0;
    }

    if (len > count)
    {
        len = count;
    }

    if (_jm_v1_box_w80_read_bytes(self, receive_buffer, len) != len)
    {
        return 0;
    }

    if (_jm_v1_box_w80_read_bytes(self, &cs, 1) != 1)
    {
        return 0;
    }

    return len;
}

static gboolean _jm_v1_box_w80_do_cmd(JMV1Box *self, guint8 cmd, 
    size_t count, const guint8 *buff)
{
    JMV1BoxW80 *w80 = NULL;
    gboolean ret = FALSE;
    g_return_val_if_fail(self != NULL, FALSE);
    w80 = (JMV1BoxW80*)self->user_data;

    w80->start_pos = 0;

    if (cmd != JM_V1_BOX_W80_WR_DATA && cmd != JM_V1_BOX_W80_SEND_DATA)
    {
        cmd |= count; // 加上长度位 
    }

    if (self->shared->is_do_now)
    {
        // 发送到BOX执行 
        if (cmd == JM_V1_BOX_W80_WR_DATA)
        {
            GByteArray *temp = NULL;
            if (count == 0)
            {
                return FALSE;
            }

            temp = g_byte_array_new();
            if (w80->is_link)
            {
                cmd = 0xFF; // 写链路保持 
            }
            else
            {
                cmd = 0x00; // 写通讯命令 
            }
            g_byte_array_append(temp, &cmd, 1);
            cmd = count;
            g_byte_array_append(temp, &cmd, 1);
            g_byte_array_append(temp, buff, count);
            ret = _jm_v1_box_w80_send_cmd(self, JM_V1_BOX_W80_WR_DATA, 
                temp->len, temp->data);
            g_byte_array_free(temp, TRUE);
            return ret;
        }
        else if (cmd == JM_V1_BOX_W80_SEND_DATA)
        {
            GByteArray *temp = NULL;
            if (count == 0)
                return FALSE;
            temp = g_byte_array_new();
            cmd = 0x00; // 写入位置 
            g_byte_array_append(temp, &cmd, 1);
            cmd = count + 2;
            g_byte_array_append(temp, &cmd, 1); // 数据包长度 
            cmd = JM_V1_BOX_W80_SEND_DATA; // 命令 
            g_byte_array_append(temp, &cmd, 1);
            cmd = count - 1; // 命令长度-1 
            g_byte_array_append(temp, &cmd, 1);
            g_byte_array_append(temp, buff, count);
            ret = _jm_v1_box_w80_send_cmd(self, JM_V1_BOX_W80_WR_DATA, 
                temp->len, temp->data);
            g_byte_array_free(temp, TRUE);
            if (!ret)
            {
                return FALSE;
            }
            return _jm_v1_box_w80_send_cmd(self, JM_V1_BOX_W80_DO_BAT_C, 0,
                NULL);
        }
        else
        {
            return _jm_v1_box_w80_send_cmd(self, cmd, count, buff);
        }
    }
    else
    {
        // 写命令到缓冲区 
        w80->buf[w80->pos++] = cmd;
        if (cmd == JM_V1_BOX_W80_SEND_DATA)
        {
            w80->buf[w80->pos++] = count - 1;
        }
        w80->start_pos = w80->pos;
        if (count > 0)
        {
            memcpy(w80->buf + w80->pos, buff, count);
        }
        w80->pos += count;
        return TRUE;
    }
    return FALSE;
}

static gboolean _jm_v1_box_w80_do_set(JMV1Box *self, guint8 cmd, 
    size_t count, const guint8 *buff)
{
    gboolean result = FALSE;

    g_return_val_if_fail(self != NULL, FALSE);

    result = _jm_v1_box_w80_do_cmd(self, cmd, count, buff);
    if (result && self->shared->is_do_now)
    {
        result = _jm_v1_box_w80_check_result(self, JM_TIMER_TO_MS(150));
    }
    return result;
}

static guint8 _jm_v1_box_w80_get_buff_data(JMV1Box *self, guint8 addr, 
    guint8 *buff, size_t count)
{
    guint8 temp[2];
    g_return_val_if_fail(self != NULL, 0);
    temp[0] = addr;
    temp[1] = count;

    if (!_jm_v1_box_w80_do_cmd(self, JM_V1_BOX_W80_GET_BUF, 2, temp))
    {
        return 0;
    }

    return _jm_v1_box_w80_get_cmd_data(self, buff, count);
}

static gboolean _jm_v1_box_w80_init_box(JMV1Box *self)
{
    static guint8 password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 
        0x17, 0x2D, 0x4D};
    JMV1BoxW80 *w80 = NULL;
    guint8 buf[32];
    size_t i;
    guint8 run = 0;

    g_return_val_if_fail(self != NULL, FALSE);
    w80 = (JMV1BoxW80*)self->user_data;

    srand((guint8)time(NULL));
    self->shared->is_do_now = TRUE;
    w80->run_flag = 0;

    for (i = 0; i < 4; i++)
    {
        buf[i] = JM_LOW_BYTE(rand());
    }

    for (i = 0; i < 10; i++)
    {
        run += password[i] ^ buf[i % 3 + 1];
    }

    if (run == 0)
    {
        run = 0x55;
    }

    if (!_jm_v1_box_w80_do_cmd(self, JM_V1_BOX_W80_GET_CPU, 3, buf + 1))
    {
        return FALSE;
    }
    if (_jm_v1_box_w80_get_cmd_data(self, buf, 32) <= 0)
    {
        return FALSE;
    }
    w80->run_flag = 0; // Run 
    w80->box_time_unit = 0;
    for (i = 0; i < 3; i++)
    {
        w80->box_time_unit = (w80->box_time_unit << 8) | buf[i];
    }
    w80->time_base_db = buf[i++];
    w80->time_extern_db = buf[i++];

    for (i = 0; i < JM_V1_BOX_W80_MAXPORT_NUM; i++)
    {
        w80->ports[i] = 0xFF;
    }

    w80->pos = 0;
    self->shared->is_db_20 = FALSE;
    return TRUE;
}

static gboolean _jm_v1_box_w80_check_box(JMV1Box *self)
{
    guint8 buf[32];
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;
    if (!_jm_v1_box_w80_do_cmd(self, JM_V1_BOX_W80_GET_BOXID, 0, NULL))
    {
        return FALSE;
    }

    if (_jm_v1_box_w80_get_cmd_data(self, buf, 32) <= 0)
    {
        return FALSE;
    }

    w80->box_ver = (buf[10] << 8) | buf[11];
    return TRUE;
}

static gboolean _jm_v1_box_w80_set_line_level(JMV1Box *self, 
    guint8 value_low, guint8 value_high)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    w80->ports[1] &= ~value_low;
    w80->ports[1] |= value_high;

    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SET_PORT1, 1, 
        w80->ports + 1);
}

static gboolean _jm_v1_box_w80_set_comm_ctrl(JMV1Box *self, 
    guint8 value_open, guint8 value_close)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    w80->ports[2] &= ~value_open;
    w80->ports[2] |= value_close;
    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SET_PORT2, 1, 
        w80->ports + 2);
}

static gboolean _jm_v1_box_w80_set_comm_line(JMV1Box *self, 
    guint8 send_line, guint8 recv_line)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    if (send_line > 7)
    {
        send_line = 0x0F;
    }

    if (recv_line > 7)
    {
        recv_line = 0x0F;
    }

    w80->ports[0] = send_line | (recv_line << 4);
    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SET_PORT0, 1, 
        w80->ports);
}

static gboolean _jm_v1_box_w80_turn_over_one_by_one(JMV1Box *self)
{
    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_ONEBYONE, 0, NULL);
}

static gboolean _jm_v1_box_w80_keep_link(JMV1Box *self, 
    gboolean is_run_link)
{
    if (is_run_link)
    {
        return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_RUN_LINK, 0, 
            NULL);
    }
    else
    {
        return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_STOP_LINK, 0, 
            NULL);
    }
}

static gboolean _jm_v1_box_w80_set_comm_link(JMV1Box *self, 
    guint8 ctrl_word1, guint8 ctrl_word2, guint8 ctrl_word3)
{
    guint8 ctrl_word[3];
    guint8 mode_control = ctrl_word1 & 0xE0;
    size_t length = 3;

    g_return_val_if_fail(self != NULL, FALSE);

    ctrl_word[0] = ctrl_word1;
    if ((ctrl_word1 & 0x04) != 0)
    {
        self->shared->is_db_20 = TRUE;
    }
    else
    {
        self->shared->is_db_20 = FALSE;
    }

    if (mode_control == JM_V1_BOX_W80_SET_VPW || 
        mode_control == JM_V1_BOX_W80_SET_PWM)
    {
        return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SET_CTRL, 1, 
            ctrl_word);
    }
    ctrl_word[1] = ctrl_word2;
    ctrl_word[2] = ctrl_word3;
    if (ctrl_word3 == 0)
    {
        length--;
        if (ctrl_word2 == 0)
        {
            length--;
        }
    }

    if (mode_control == JM_V1_BOX_W80_EXRS_232 && length < 2)
    {
        return FALSE;
    }

    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SET_CTRL, length, 
        ctrl_word);
}

static gboolean _jm_v1_box_w80_set_comm_baud(JMV1Box *self, gdouble baud)
{
    guint8 baud_time[2];
    gdouble instruct_num = 0;
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    instruct_num = ((1000000.0 / w80->box_time_unit) * 1000000) / baud;

    if (self->shared->is_db_20)
    {
        instruct_num /= 20;
    }

    instruct_num += 0.5;
    if (instruct_num > 65535 || instruct_num < 10)
    {
        return FALSE;
    }

    baud_time[0] = JM_HIGH_BYTE(instruct_num);
    baud_time[1] = JM_LOW_BYTE(instruct_num);

    if (baud_time[0] == 0)
    {
        return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SET_BAUD, 1, 
            baud_time + 1);
    }
    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SET_BAUD, 2, 
        baud_time);
}

static void _jm_v1_box_w80_get_link_time(JMV1Box *self, guint8 type, 
    guint32 time)
{
    g_return_if_fail(self != NULL);

    if (type == JM_V1_BOX_W80_SETBYTETIME)
    {
        self->shared->req_byte_to_byte = time;
    }
    else if (type == JM_V1_BOX_W80_SETRECBBOUT)
    {
        self->shared->req_wait_time = time;
    }
    else if (type == JM_V1_BOX_W80_SETRECFROUT)
    {
        self->shared->res_byte_to_byte = time;
    }
    else
    {
        self->shared->res_wait_time = time;
    }
}

static gboolean _jm_v1_box_w80_set_comm_time(JMV1Box *self, guint8 type, 
    guint32 time)
{
    guint8 time_buff[2];
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    _jm_v1_box_w80_get_link_time(self, type, time);

    if (type == JM_V1_BOX_W80_SETVPWSTART || 
        type == JM_V1_BOX_W80_SETVPWRECS)
    {
        if (type == JM_V1_BOX_W80_SETVPWRECS)
        {
            time = (time * 2) / 3;
        }
        type = type + (JM_V1_BOX_W80_SETBYTETIME & 0xF0);
        time = (guint32)(time / (w80->box_time_unit / 1000000.0));
    }
    else
    {
        time = (guint32)((time / w80->time_base_db) / 
            (w80->box_time_unit / 1000000.0));
    }

    time_buff[0] = JM_HIGH_BYTE(time);
    time_buff[1] = JM_LOW_BYTE(time);
    if (time_buff[0] == 0)
    {
        return _jm_v1_box_w80_do_set(self, type, 1, time_buff + 1);
    }
    return _jm_v1_box_w80_do_set(self, type, 2, time_buff);
}

static gboolean _jm_v1_box_w80_commbox_delay(JMV1Box *self, guint32 time)
{
    guint8 time_buff[2];
    guint8 delay_word = JM_V1_BOX_W80_DELAYSHORT;
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    time = (guint32)(time / (w80->box_time_unit / 1000000.0));

    if (time == 0)
    {
        return FALSE;
    }

    if (time > 65535)
    {
        time = (guint32)(time / w80->time_base_db);

        if (time > 65535)
        {
            time = (guint32)((time * w80->time_base_db) / 
                w80->time_extern_db);
            if (time > 65535)
            {
                return FALSE;
            }
            delay_word = JM_V1_BOX_W80_DELAYDWORD;
        }
        else
        {
            delay_word = JM_V1_BOX_W80_DELAYTIME;
        }
    }
    time_buff[0] = JM_HIGH_BYTE(time);
    time_buff[1] = JM_LOW_BYTE(time);

    if (time_buff[0] == 0)
    {
        return _jm_v1_box_w80_do_set(self, delay_word, 1, time_buff + 1);
    }
    return _jm_v1_box_w80_do_set(self, delay_word, 2, time_buff);
}

static gboolean _jm_v1_box_w80_send_out_data(JMV1Box *self, 
    const guint8 *buffer, size_t count)
{
    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_SEND_DATA, count, 
        buffer);
}

static gboolean _jm_v1_box_w80_run_receive(JMV1Box *self, guint8 type)
{
    g_return_val_if_fail(self != NULL, FALSE);
    if (type == JM_V1_BOX_W80_GET_PORT1)
    {
        self->shared->is_db_20 = FALSE;
    }
    return _jm_v1_box_w80_do_cmd(self, type, 0, NULL);
}

static gboolean _jm_v1_box_w80_stop_now(JMV1Box *self, 
    gboolean is_stop_execute)
{
    guint8 cmd;
    size_t i;

    if (is_stop_execute)
    {
        cmd = JM_V1_BOX_W80_STOP_EXECUTE;
    }
    else
    {
        cmd = JM_V1_BOX_W80_STOP_REC;
    }

    for (i = 0; i < 3; i++)
    {
        if (jm_commbox_port_write(&cmd, 1) != 1)
            continue;

        if (_jm_v1_box_w80_check_send(self))
        {
            if (is_stop_execute && !_jm_v1_box_w80_check_result(self, 
                JM_TIMER_TO_MS(200)))
                continue;
            return TRUE;
        }
    }
    return FALSE;
}

static gboolean _jm_v1_box_w80_set_rf(JMV1Box *self, guint8 cmd, 
    guint8 data)
{
    return FALSE;
}

static gboolean _jm_v1_box_w80_open_box(JMV1Box *self)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);
    if (_jm_v1_box_w80_init_box(self) && _jm_v1_box_w80_check_box(self))
    {
        jm_commbox_port_discard_in_buffer();
        jm_commbox_port_discard_out_buffer();
        w80 = (JMV1BoxW80*)self->user_data;
        w80->is_initialize = FALSE;
        return TRUE;
    }
    return FALSE;
}

static gboolean _jm_v1_box_w80_open_comm(JMV1Box *self)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;
    w80->is_initialize = TRUE;
    return _jm_v1_box_w80_open_box(self);
}

static gboolean _jm_v1_box_w80_reset(JMV1Box *self)
{
    size_t i;
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    _jm_v1_box_w80_stop_now(self, TRUE);
    jm_commbox_port_discard_out_buffer();
    jm_commbox_port_discard_in_buffer();

    for (i = 0; i < JM_V1_BOX_W80_MAXPORT_NUM; i++)
    {
        w80->ports[i] = 0xFF;
    }

    return _jm_v1_box_w80_do_cmd(self, JM_V1_BOX_W80_RESET, 0, NULL);
}

static gboolean _jm_v1_box_w80_close_comm(JMV1Box *self)
{
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    _jm_v1_box_w80_reset(self);
    _jm_v1_box_w80_set_rf(self, JM_V1_BOX_W80_RF_RESET, 0);
    w80->is_initialize = FALSE;
    return TRUE;
}

static guint8 _jm_v1_box_w80_get_abs_add(JMV1Box *self, guint8 buff_id, 
    guint8 add)
{
    return 0;
}

static gboolean _jm_v1_box_w80_update_buff(JMV1Box *self, guint8 type, 
    guint8 *buffer)
{
    int len = 0;
    guint8 buf[3];
    buf[0] = buffer[0];
    buf[1] = buffer[1];

    if ((type == JM_V1_BOX_W80_INC_BYTE) || 
        (type == JM_V1_BOX_W80_DEC_BYTE) || 
        (type == JM_V1_BOX_W80_INVERTBYTE))
    {
        len = 1;
    }
    else if ((type == JM_V1_BOX_W80_UPDATE_BYTE) || 
        (type == JM_V1_BOX_W80_ADD_BYTE) || 
        (type == JM_V1_BOX_W80_SUB_BYTE))
    {
        len = 2;
    }
    else
    {
        len = 3;
    }
    return _jm_v1_box_w80_do_set(self, type, len, buf);
}

static gboolean _jm_v1_box_w80_copy_buff(JMV1Box *self, guint8 dest, 
    guint8 src, guint8 len)
{
    guint8 buf[3];

    buf[0] = dest;
    buf[1] = src;
    buf[2] = len;

    return _jm_v1_box_w80_do_set(self, JM_V1_BOX_W80_COPY_BYTE, 3, buf);
}

static gboolean _jm_v1_box_w80_new_batch(JMV1Box *self, guint8 buff_id)
{
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    w80->pos = 0;
    w80->is_link = (buff_id == JM_V1_BOX_W80_LINKBLOCK ? TRUE : FALSE);
    self->shared->is_do_now = FALSE;
    return TRUE;
}

static gboolean _jm_v1_box_w80_end_batch(JMV1Box *self)
{
    int i = 0;
    JMV1BoxW80 *w80 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    self->shared->is_do_now = TRUE;
    w80->buf[w80->pos++] = 0; // 命令块以0x00标记结束 
    if (w80->is_link)
    {
        // 修改update_buff使用到的地址 
        while (w80->buf[i] != 0)
        {
            guint8 mode = w80->buf[i] & 0xFC;
            if (mode == JM_V1_BOX_W80_COPY_BYTE)
            {
                w80->buf[i + 3] += JM_V1_BOX_W80_MAXBUFF_LEN - w80->pos;
            }
            else if (mode == JM_V1_BOX_W80_SUB_BYTE)
            {
                w80->buf[i + 2] += JM_V1_BOX_W80_MAXBUFF_LEN - w80->pos;
            }
            else
            {
                w80->buf[i + 1] += JM_V1_BOX_W80_MAXBUFF_LEN - w80->pos;
            }

            if (w80->buf[i] == JM_V1_BOX_W80_SEND_DATA)
            {
                i += 1 + w80->buf[i + 1] + 1 + 1;
            }
            else if(w80->buf[i] >= JM_V1_BOX_W80_REC_LEN_1 && 
                w80->buf[i] <= JM_V1_BOX_W80_REC_LEN_15)
            {
                i++; // 特殊 
            }
            else
            {
                i = 1 + ((w80->buf[i] & 0x03) + 1);
            }
        }
    }

    return _jm_v1_box_w80_do_cmd(self, JM_V1_BOX_W80_WR_DATA, w80->pos, 
        w80->buf);
}

static gboolean _jm_v1_box_w80_del_batch(JMV1Box *self, guint8 buff_id)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;
    self->shared->is_do_now = TRUE;

    w80->pos = 0;
    return TRUE;

}

static gboolean _jm_v1_box_w80_run_batch(JMV1Box *self, guint8 *buff_id, 
    size_t count, gboolean is_execute_many)
{
    guint8 cmd;

    if (buff_id[0] == JM_V1_BOX_W80_LINKBLOCK)
    {
        if (is_execute_many)
        {
            cmd = JM_V1_BOX_W80_DO_BAT_LN;
        }
        else
        {
            cmd = JM_V1_BOX_W80_DO_BAT_L;
        }
    }
    else
    {
        if (is_execute_many)
        {
            cmd = JM_V1_BOX_W80_DO_BAT_CN;
        }
        else
        {
            cmd = JM_V1_BOX_W80_DO_BAT_C;
        }
    }
    return _jm_v1_box_w80_do_cmd(self, cmd, 0, NULL);
}

static guint32 _jm_v1_box_w80_get_box_ver(JMV1Box *self)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, 0);

    w80 = (JMV1BoxW80*)self->user_data;

    return w80->box_ver;
}

static gint32 _jm_v1_box_w80_serial_port_baud(JMV1Box *self)
{
    return 115200;
}

static guint8 _jm_v1_box_w80_serial_port_databits(JMV1Box *self)
{
    return 8;
}

static gint32 _jm_v1_box_w80_serial_port_parity(JMV1Box *self)
{
    return JM_SP_PAR_NONE;
}

static gint32 _jm_v1_box_w80_serial_port_stopbits(JMV1Box *self)
{
    return JM_SP_SB_TWO;
}

static gint32 _jm_v1_box_w80_serial_port_flow_control(JMV1Box *self)
{
    return JM_SP_FC_NONE;
}

static gboolean _jm_v1_box_w80_serial_port_change_config(JMV1Box *self)
{
    JMV1BoxW80 *w80 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);

    w80 = (JMV1BoxW80*)self->user_data;

    return w80->is_initialize;
}

static gboolean _jm_v1_box_w80_check_serial_port_change_config(
    JMV1Box *self)
{
    return TRUE;
}

JMV1Box* jm_v1_box_w80_new(JMV1Shared *shared)
{
    JMV1Box *obj = jm_v1_box_new(shared);
    JMV1BoxW80 *b = (JMV1BoxW80*)g_malloc(sizeof(JMV1BoxW80));

    b->is_initialize = TRUE;

    obj->user_data = b;

    obj->check_result = _jm_v1_box_w80_check_result;
    obj->check_serial_port_change_config = 
        _jm_v1_box_w80_check_serial_port_change_config;
    obj->close_comm = _jm_v1_box_w80_close_comm;
    obj->commbox_delay = _jm_v1_box_w80_commbox_delay;
    obj->del_batch = _jm_v1_box_w80_del_batch;
    obj->end_batch = _jm_v1_box_w80_end_batch;
    obj->get_abs_add = _jm_v1_box_w80_get_abs_add;
    obj->keep_link = _jm_v1_box_w80_keep_link;
    obj->new_batch = _jm_v1_box_w80_new_batch;
    obj->open_comm = _jm_v1_box_w80_open_comm;
    obj->read_bytes = _jm_v1_box_w80_read_bytes;
    obj->read_data = _jm_v1_box_w80_read_data;
    obj->run_batch = _jm_v1_box_w80_run_batch;
    obj->run_receive = _jm_v1_box_w80_run_receive;
    obj->send_out_data = _jm_v1_box_w80_send_out_data;
    obj->serial_port_baud = _jm_v1_box_w80_serial_port_baud;
    obj->serial_port_change_config = 
        _jm_v1_box_w80_serial_port_change_config;
    obj->serial_port_databits = _jm_v1_box_w80_serial_port_databits;
    obj->serial_port_flow_control = 
        _jm_v1_box_w80_serial_port_flow_control;
    obj->serial_port_parity = _jm_v1_box_w80_serial_port_parity;
    obj->serial_port_stopbits = _jm_v1_box_w80_serial_port_stopbits;
    obj->set_comm_baud = _jm_v1_box_w80_set_comm_baud;
    obj->set_comm_time = _jm_v1_box_w80_set_comm_time;
    obj->set_comm_ctrl = _jm_v1_box_w80_set_comm_ctrl;
    obj->set_comm_line = _jm_v1_box_w80_set_comm_line;
    obj->set_comm_link = _jm_v1_box_w80_set_comm_link;
    obj->set_line_level = _jm_v1_box_w80_set_line_level;
    obj->stop_now = _jm_v1_box_w80_stop_now;
    obj->turn_over_one_by_one = _jm_v1_box_w80_turn_over_one_by_one;
    obj->update_buff = _jm_v1_box_w80_update_buff;

    obj->box_type = JM_V1_BOX_W80;

    return obj;
}

void jm_v1_box_w80_free(JMV1Box *self)
{
    g_return_if_fail(self != NULL);

    g_free(self->user_data);
    self->user_data = NULL;

    jm_v1_box_free(self);
}
