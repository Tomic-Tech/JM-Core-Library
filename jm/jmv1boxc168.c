#include <stdlib.h>
#include <string.h>
#include "jmv1boxc168.h"
#include "jmv1boxc168d.h"
#include "jmcommboxport.h"
#include "jmtimer.h"
#include "jmutils.h"
#include "jmserialport.h"

typedef struct _JMV1BoxC168 JMV1BoxC168;

struct _JMV1BoxC168 {
    gint32 serial_port_baud;
    guint32 buff_used_num;
    guint32 time_unit;
    guint32 time_base_db;
    guint32 time_extern_db;
    guint32 cmd_buff_len;
    gboolean is_initialize;
    guint8 buff_id;
    guint8 head_password;
    guint8 buff_add[JM_V1_BOX_C168_MAXIM_BLOCK + 2];
    guint8 buff_used[JM_V1_BOX_C168_MAXIM_BLOCK];
    guint8 version[JM_V1_BOX_C168_VERSIONLEN];
    guint8 box_id[JM_V1_BOX_C168_COMMBOXIDLEN];
    guint8 port[JM_V1_BOX_C168_COMMBOXPORTNUM];
    guint8 cmd_temp[256]; // 写入命令缓冲区 
};

static gboolean _jm_v1_box_c168_check_idle(JMV1Box *self) {
    guint8 receive_buffer = JM_V1_BOX_C168_SUCCESS;
    size_t avail;

    g_return_val_if_fail(self != NULL, FALSE);

    avail = jm_commbox_port_bytes_available();

    if (avail > 240) {
        jm_commbox_port_discard_in_buffer();
        jm_commbox_port_discard_out_buffer();
        return TRUE;
    }

    while (avail) {
        jm_commbox_port_read(&receive_buffer, 1);
        avail--;
    }

    if (receive_buffer == JM_V1_BOX_C168_SUCCESS) {
        return TRUE;
    }

    jm_commbox_port_set_read_timeout(JM_TIMER_TO_MS(200));

    if (jm_commbox_port_read(&receive_buffer, 1) != 1) {
        self->shared->last_error = JM_V1_BOX_C168_KEEPLINK_ERROR;
        return FALSE;
    }

    if (receive_buffer == JM_V1_BOX_C168_SUCCESS) {
        return TRUE;
    }

    self->shared->last_error = receive_buffer;
    return FALSE;
}

static gboolean _jm_v1_box_c168_send_ok(JMV1Box *self, gint64 microseconds) {
    guint8 receive_buffer = 0;
    g_return_val_if_fail(self != NULL, FALSE);

    jm_commbox_port_set_read_timeout(microseconds);

    while (TRUE) {
        if (jm_commbox_port_read(&receive_buffer, 1) != 1) {
            self->shared->last_error = JM_V1_BOX_C168_TIMEOUT_ERROR;
            return FALSE;
        }

        if (receive_buffer == JM_V1_BOX_C168_SEND_OK) {
            return TRUE;
        } else if (receive_buffer >= JM_V1_BOX_C168_UP_TIMEOUT && receive_buffer <= JM_V1_BOX_C168_ERROR_REC) {
            self->shared->last_error = JM_V1_BOX_C168_SENDDATA_ERROR;
            return FALSE;
        }
    }

    self->shared->last_error = JM_V1_BOX_C168_SENDDATA_ERROR;
    return FALSE;
}

static guint32 _jm_v1_box_c168_get_box_ver(JMV1Box *self) {
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, 0);

    c168 = (JMV1BoxC168*)self->user_data;

    return (c168->version[0] << 8) | (c168->version[1]);
}

static size_t _jm_v1_box_c168_buffer_size(JMV1Box *self) {
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, 0);

    c168 = (JMV1BoxC168*)self->user_data;

    return c168->buff_add[JM_V1_BOX_C168_LINKBLOCK] - c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
}

static gboolean _jm_v1_box_c168_commbox_do(JMV1Box *self, guint8 command_word, size_t count, const guint8 *buff);

static gboolean _jm_v1_box_c168_send_data_to_ecu_new(JMV1Box *self, size_t count, const guint8 *buff) {
    size_t i;
    size_t checksum = count + 5;
    size_t size = count + 6;
    guint8 *command = (guint8*)g_malloc(size);
    JMV1BoxC168 *c168 = (JMV1BoxC168*)self->user_data;

    command[0] = JM_V1_BOX_C168_WR_DATA + c168->head_password;
    command[1] = JM_LOW_BYTE(count + 3);
    command[2] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
    command[3] = JM_V1_BOX_C168_SEND_CMD;
    command[4] = JM_LOW_BYTE(count - 1);

    command[checksum] = JM_V1_BOX_C168_WR_DATA + command[1] + command[2] + command[3] + command[4];

    memcpy(command + 5, buff, count);

    for(i = 0; i < count; i++) {
        command[checksum] += buff[i];
    }

    for (i = 0; i < 3; i++) {
        if (!_jm_v1_box_c168_check_idle(self) || (jm_commbox_port_write(command, size) != size)) {
            self->shared->last_error = JM_V1_BOX_C168_SENDDATA_ERROR;
            continue;
        }

        if (_jm_v1_box_c168_send_ok(self, 20 * (count + 12))) {
            g_free(command);
            return TRUE;
        }
    }

    g_free(command);
    return FALSE;
}

static gboolean _jm_v1_box_c168_send_data_to_ecu_old(JMV1Box *self, size_t count, const guint8 *buff) {
    size_t i;
    size_t checksum = count + 4;
    size_t size = count + 5;
    guint8 *command = (guint8*)g_malloc(size);
    JMV1BoxC168 *c168 = (JMV1BoxC168*)self->user_data;

    command[0] = JM_V1_BOX_C168_WR_DATA + c168->head_password;
    command[1] = JM_LOW_BYTE(count + 2);
    command[2] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
    command[3] = JM_LOW_BYTE(count - 1);

    command[checksum] = JM_V1_BOX_C168_WR_DATA + command[1] + command[2] + command[3];

    memcpy(command + 4, buff, count);

    for (i = 0; i < count; i++) {
        command[checksum] += buff[i];
    }

    for (i = 0; i < 3; i++) {
        if (!_jm_v1_box_c168_check_idle(self) || (jm_commbox_port_write(command, size) != size)) {
            self->shared->last_error = JM_V1_BOX_C168_SENDDATA_ERROR;
            continue;
        }

        if (_jm_v1_box_c168_send_ok(self, 20 * (size + 6))) {
            g_free(command);
            return TRUE;
        }
    }

    g_free(command);
    return FALSE;

}

static gboolean _jm_v1_box_c168_send_data_to_ecu(JMV1Box *self, guint8 command_word, size_t count, const guint8 *buff) {
    JMV1BoxC168 *c168 = (JMV1BoxC168*)self->user_data;
    if (command_word == JM_V1_BOX_C168_SEND_DATA && count <= JM_V1_BOX_C168_SEND_LEN) {
        if (_jm_v1_box_c168_buffer_size(self) < (count + 1)) {
            self->shared->last_error = JM_V1_BOX_C168_NOBUFF_TOSEND;
            return FALSE;
        }

        if (_jm_v1_box_c168_get_box_ver(self) > 0x400) {
            // 增加发送长命令 
            if (!_jm_v1_box_c168_send_data_to_ecu_new(self, count, buff)) {
                return FALSE;
            }
        } else {
            // 保持与旧盒子兼容 
            if (!_jm_v1_box_c168_send_data_to_ecu_old(self, count, buff)) {
                return FALSE;
            }
        }
        return _jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_D0_BAT, 1, c168->buff_add + JM_V1_BOX_C168_SWAPBLOCK);
    }
    self->shared->last_error = JM_V1_BOX_C168_ILLIGICAL_LEN;
    return FALSE;
}

static gboolean _jm_v1_box_c168_commbox_command(JMV1Box *self, guint8 command_word, size_t count, const guint8 *buff) {
    size_t size = count + 2;
    size_t checksum = count + 1;
    size_t i;
    guint8 *command = (guint8*)g_malloc(size);
    JMV1BoxC168 *c168 = (JMV1BoxC168*)self->user_data;

    command[checksum] = JM_LOW_BYTE(command_word + count);

    if (command_word < JM_V1_BOX_C168_WR_DATA) {
        if (count == 0) {
            self->shared->last_error = JM_V1_BOX_C168_ILLIGICAL_LEN;
            g_free(command);
            return FALSE;
        }
        checksum--;
    } else {
        if (count != 0) {
            self->shared->last_error = JM_V1_BOX_C168_ILLIGICAL_LEN;
            g_free(command);
            return FALSE;
        }
    }

    command[0] = checksum + c168->head_password;
    memcpy(command + 1, buff, count);

    for (i = 0; i < count; i++) {
        command[checksum] += buff[i];
    }

    for (i = 0; i < 3; i++) {
        if (command_word != JM_V1_BOX_C168_STOP_REC && command_word != JM_V1_BOX_C168_STOP_EXECUTE) {
            if (!_jm_v1_box_c168_check_idle(self) || (jm_commbox_port_write(command, size) != size)) {
                self->shared->last_error = JM_V1_BOX_C168_SENDDATA_ERROR;
                continue;
            }
        } else {
            if (jm_commbox_port_write(command, size) != size) {
                self->shared->last_error = JM_V1_BOX_C168_SENDDATA_ERROR;
                continue;
            }
        }

        if (_jm_v1_box_c168_send_ok(self, JM_TIMER_TO_MS(20) * size)) {
            g_free(command);
            return TRUE;
        }
    }

    g_free(command);
    return FALSE;
}

static gboolean _jm_v1_box_c168_commbox_do(JMV1Box* self, guint8 command_word, size_t count, const guint8 *buff) {
    g_return_val_if_fail(self != NULL, FALSE);
    if (count > JM_V1_BOX_C168_CMD_DATALEN) {
        return _jm_v1_box_c168_send_data_to_ecu(self, command_word, count, buff);
    } else {
        return _jm_v1_box_c168_commbox_command(self, command_word, count, buff);
    }
}

static gboolean _jm_v1_box_c168_check_result(JMV1Box *self, gint64 microseconds) {
    guint8 receive_buffer;
    g_return_val_if_fail(self != NULL, FALSE);

    jm_commbox_port_set_read_timeout(microseconds);

    if (jm_commbox_port_read(&receive_buffer, 1) != 1) {
        self->shared->last_error = JM_V1_BOX_C168_TIMEOUT_ERROR;
    }

    if (receive_buffer == JM_V1_BOX_C168_SUCCESS) {
        return TRUE;
    }

    while (jm_commbox_port_read(&receive_buffer, 1) == 1);
    self->shared->last_error = receive_buffer;
    return FALSE;
}

static gboolean _jm_v1_box_c168_stop_now(JMV1Box *self, gboolean is_stop_execute) {
    g_return_val_if_fail(self != NULL, FALSE);
    if (is_stop_execute) {
        guint8 receive_buffer = 0;
        guint32 times = JM_V1_BOX_C168_REPLAYTIMES;
        while (times--) {
            if (!_jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_STOP_EXECUTE, 0, NULL)) {
                return FALSE;
            }
            if (jm_commbox_port_read(&receive_buffer, 1) != 1) {
                self->shared->last_error = JM_V1_BOX_C168_TIMEOUT_ERROR;
            }
            if (receive_buffer == JM_V1_BOX_C168_RUN_ERR) {
                return TRUE;
            }
            self->shared->last_error = JM_V1_BOX_C168_TIMEOUT_ERROR;
        }
        return FALSE;
    }
    return _jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_STOP_REC, 0, NULL);
}

static gboolean _jm_v1_box_c168_do_set(JMV1Box *self, guint8 command_word, size_t count, const guint8 *buff) {
    guint32 times = JM_V1_BOX_C168_REPLAYTIMES;
    g_return_val_if_fail(self != NULL, FALSE);

    while (times) {
        if (!_jm_v1_box_c168_commbox_do(self, command_word, count, buff)) {
            continue;
        }
        if (_jm_v1_box_c168_check_result(self, JM_TIMER_TO_MS(50))) {
            return TRUE;
        }
        _jm_v1_box_c168_stop_now(self, TRUE);
    }
    return FALSE;
}

static size_t _jm_v1_box_c168_read_data(JMV1Box *self, guint8 *buff, size_t count, gint64 microseconds) {
    jm_commbox_port_set_read_timeout(microseconds);

    return jm_commbox_port_read(buff, count);
}

static size_t _jm_v1_box_c168_get_cmd_data(JMV1Box *self, guint8 command, guint8 *receive_buffer) {
    guint8 checksum = command;
    guint8 cmd_info[2];
    size_t i;

    g_return_val_if_fail(self != NULL, 0);

    if (_jm_v1_box_c168_read_data(self, cmd_info, 2, JM_TIMER_TO_MS(150)) != 2) {
        return 0;
    }

    if (cmd_info[0] != command) {
        self->shared->last_error = cmd_info[0];
        jm_commbox_port_discard_in_buffer();
        return 0;
    }

    if ((_jm_v1_box_c168_read_data(self, receive_buffer, cmd_info[1], JM_TIMER_TO_MS(150)) != cmd_info[1]) ||
        (_jm_v1_box_c168_read_data(self, cmd_info, 1, JM_TIMER_TO_MS(150)) != 1)) {
            return 0;
    }

    checksum += cmd_info[1];

    for (i = 0; i < cmd_info[1]; i++) {
        checksum += receive_buffer[i];
    }

    if (checksum != cmd_info[0]) {
        self->shared->last_error = JM_V1_BOX_C168_CHECKSUM_ERROR;
        return 0;
    }
    return cmd_info[1];
}

static gboolean _jm_v1_box_c168_check_box(JMV1Box *self) {
    static guint8 password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 0x17, 0x2D, 0x4D};
    guint8 temp[5];
    size_t i;
    guint8 checksum;
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    srand((guint32)time(NULL));

    temp[4] = 0x00;
    i = 0;
    while (i < 4) {
        temp[i] = JM_LOW_BYTE(rand());
        temp[4] += temp[i];
        i++;
    }

    if (jm_commbox_port_write(temp, 5) != 5) {
        self->shared->last_error = JM_V1_BOX_C168_SENDDATA_ERROR;
        return FALSE;
    }

    i = 0;
    checksum = temp[4] + temp[4];

    while (i < sizeof(password)) {
        checksum += password[i] ^ temp[i % 5];
        i++;
    }

    g_usleep(JM_TIMER_TO_MS(20));

    if (_jm_v1_box_c168_get_cmd_data(self, JM_V1_BOX_C168_GETINFO, temp) <= 0) {
        return FALSE;
    }

    c168 = (JMV1BoxC168*)self->user_data;
    c168->head_password = temp[0];

    if (checksum != c168->head_password) {
        self->shared->last_error = JM_V1_BOX_C168_CHECKSUM_ERROR;
        return FALSE;
    }
    if (c168->head_password == 0) {
        c168->head_password = 0x55;
    }
    return TRUE;
}

static gboolean _jm_v1_box_c168_init_box(JMV1Box *self) {
    guint32 length;
    JMV1BoxC168 *c168 = NULL;
    size_t pos = 0;
    size_t i;
    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;
    self->shared->is_db_20 = FALSE;

    if (!_jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_GETINFO, 0, NULL)) {
        return FALSE;
    }

    length = _jm_v1_box_c168_get_cmd_data(self, JM_V1_BOX_C168_GETINFO, c168->cmd_temp);
    if (length < JM_V1_BOX_C168_COMMBOXIDLEN) {
        self->shared->last_error = JM_V1_BOX_C168_LOST_VERSIONDATA;
        return FALSE;
    }
    
    c168->time_unit = 0;
    for (i = 0; i < JM_V1_BOX_C168_MINITIMELEN; i++) {
        c168->time_unit = (c168->time_unit << 8) | c168->cmd_temp[pos++];
    }
    c168->time_base_db = c168->cmd_temp[pos++];
    c168->time_extern_db = c168->cmd_temp[pos++];
    c168->cmd_buff_len = c168->cmd_temp[pos++];
    if (c168->time_base_db == 0 ||
        c168->time_unit == 0 ||
        c168->cmd_buff_len == 0) {
            self->shared->last_error = JM_V1_BOX_C168_COMMTIME_ZERO;
            return FALSE;
    }

    for (i = 0; i < JM_V1_BOX_C168_COMMBOXIDLEN; i++) {
        c168->box_id[i] = c168->cmd_temp[pos++];
    }

    for (i = 0; i < JM_V1_BOX_C168_VERSIONLEN; i++) {
        c168->version[i] = c168->cmd_temp[pos++];
    }

    c168->port[0] = JM_V1_BOX_C168_NULLADD;
    c168->port[1] = JM_V1_BOX_C168_NULLADD;
    c168->port[2] = JM_V1_BOX_C168_NULLADD;
    c168->port[3] = JM_V1_BOX_C168_NULLADD;

    c168->buff_id = JM_V1_BOX_C168_NULLADD;
    c168->buff_used_num = 0;

    memset(c168->buff_add, JM_V1_BOX_C168_NULLADD, JM_V1_BOX_C168_MAXIM_BLOCK);
    c168->buff_add[JM_V1_BOX_C168_LINKBLOCK] = c168->cmd_buff_len;
    c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK] = 0;
    return TRUE;
}

static gboolean _jm_v1_box_c168_set_rf(JMV1Box *self, guint8 cmd, guint8 cmd_info) {
    size_t times = JM_V1_BOX_C168_REPLAYTIMES;

    cmd_info += cmd;
    if (cmd == JM_V1_BOX_C168_SETRFBAUD) {
        times = 2;
    }

    g_usleep(JM_TIMER_TO_MS(6));
    while (times--) {
        if (_jm_v1_box_c168_check_idle(self) && (jm_commbox_port_write(&cmd_info, 1) == 1)) {
            if (!_jm_v1_box_c168_send_ok(self, JM_TIMER_TO_MS(50))) {
                continue;
            }

            if ((jm_commbox_port_write(&cmd_info, 1) != 1) ||
                !_jm_v1_box_c168_check_result(self, JM_TIMER_TO_MS(150))) {
                    continue;
            }
            g_usleep(JM_TIMER_TO_MS(100));
            return TRUE;
        }
    }
    return FALSE;
}

static gboolean _jm_v1_box_c168_serial_port_change_config(JMV1Box *self) {
    JMV1BoxC168 *c168 = NULL;
    guint8 baud = JM_V1_BOX_C168_UP_57600BPS;

    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;

    if (c168->is_initialize) {
        return TRUE;
    }

    self->shared->last_error = 0;

    if (!_jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_SET_UPBAUD, 1, &baud)) {
        return FALSE;
    }

    g_usleep(JM_TIMER_TO_MS(50));
    _jm_v1_box_c168_check_result(self, JM_TIMER_TO_MS(50));
    _jm_v1_box_c168_set_rf(self, JM_V1_BOX_C168_SETRFBAUD, JM_V1_BOX_C168_UP_57600BPS);
    _jm_v1_box_c168_check_result(self, JM_TIMER_TO_MS(50));
    c168->serial_port_baud = 57600;
    return TRUE;
}

static gboolean _jm_v1_box_c168_check_serial_port_change_config(JMV1Box *self) {
    guint8 baud = JM_V1_BOX_C168_UP_57600BPS;
    g_return_val_if_fail(self != NULL, FALSE);

    _jm_v1_box_c168_set_rf(self, JM_V1_BOX_C168_SETRFBAUD, JM_V1_BOX_C168_UP_57600BPS);

    if (!_jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_SET_UPBAUD, 1, &baud)) {
        return FALSE;
    }

    if (!_jm_v1_box_c168_check_result(self, JM_TIMER_TO_MS(100))) {
        return FALSE;
    }

    jm_commbox_port_discard_in_buffer();
    jm_commbox_port_discard_out_buffer();
    return TRUE;
}

static gboolean _jm_v1_box_c168_open_comm(JMV1Box *self) {
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;
    _jm_v1_box_c168_set_rf(self, JM_V1_BOX_C168_RESET_RF, 0);
    _jm_v1_box_c168_set_rf(self, JM_V1_BOX_C168_SETDTR_L, 0);
    if (_jm_v1_box_c168_init_box(self) && _jm_v1_box_c168_check_box(self)) {
        c168->is_initialize = FALSE;
        return TRUE;
    }
    return FALSE;
}

static gboolean _jm_v1_box_c168_close_comm(JMV1Box *self) {
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;
    _jm_v1_box_c168_stop_now(self, TRUE);
    _jm_v1_box_c168_do_set(self, JM_V1_BOX_C168_RESET, 0, NULL);
    _jm_v1_box_c168_set_rf(self, JM_V1_BOX_C168_RESET_RF, 0);
    c168->is_initialize = TRUE;
    return TRUE;
}

static gboolean _jm_v1_box_c168_del_batch(JMV1Box *self, guint8 buff_id) {
    JMV1BoxC168 *c168 = NULL;
    
    g_return_val_if_fail(self != NULL, FALSE);

    if (buff_id > JM_V1_BOX_C168_MAXIM_BLOCK) {
        // 数据块不存在
        self->shared->last_error = JM_V1_BOX_C168_NODEFINE_BUFF;
        return FALSE;
    }

    c168 = (JMV1BoxC168*)self->user_data;

    if (c168->buff_id == buff_id) {
        c168->buff_id = JM_V1_BOX_C168_NULLADD;
        return TRUE;
    }

    if (c168->buff_add[buff_id] == JM_V1_BOX_C168_NULLADD) {
        // 数据块标识登记是否有申请?
        self->shared->last_error = JM_V1_BOX_C168_NOUSED_BUFF;
        return FALSE;
    }

    if (buff_id == JM_V1_BOX_C168_LINKBLOCK) {
        c168->buff_add[JM_V1_BOX_C168_LINKBLOCK] = c168->cmd_buff_len;
    } else {
        size_t i = 0;
        guint8 data[3];
        guint8 delete_buff_len;

        for (; i < c168->buff_used_num; i++) {
            if (c168->buff_used[i] == buff_id) {
                break;
            }
        }
        data[0] = c168->buff_add[buff_id];
        if (i < c168->buff_used_num - 1) {
            data[1] = c168->buff_add[c168->buff_used[i + 1]];
            data[2] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK] - data[1];

            if (!_jm_v1_box_c168_do_set(self, JM_V1_BOX_C168_COPY_DATA - JM_V1_BOX_C168_COPY_DATA % 4, 3, data)) {
                return FALSE;
            }
        } else {
            data[1] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
        }

        delete_buff_len = data[1] - data[0];
        for (i = i + 1; i < c168->buff_used_num; i++) {
            c168->buff_used[i - 1] = c168->buff_used[i];
            c168->buff_add[c168->buff_used[i]] = c168->buff_add[c168->buff_used[i]] - delete_buff_len;
        }
        c168->buff_used_num--;
        c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK] - delete_buff_len;
        c168->buff_add[buff_id] = JM_V1_BOX_C168_NULLADD;
    }
    return TRUE;
}

static gboolean _jm_v1_box_c168_new_batch(JMV1Box *self, guint8 buff_id) {
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);
    c168 = (JMV1BoxC168*)self->user_data;

    if (buff_id > JM_V1_BOX_C168_MAXIM_BLOCK) {
        self->shared->last_error = JM_V1_BOX_C168_NODEFINE_BUFF;
        return FALSE;
    }
    if (c168->buff_id != JM_V1_BOX_C168_NULLADD) {
        self->shared->last_error = JM_V1_BOX_C168_APPLICATION_NOW;
        return FALSE;
    }

    if ((c168->buff_add[buff_id] != JM_V1_BOX_C168_NULLADD && buff_id != JM_V1_BOX_C168_LINKBLOCK && !_jm_v1_box_c168_del_batch(self, buff_id))) {
        return FALSE;;
    }

    c168->cmd_temp[0] = JM_V1_BOX_C168_WR_DATA;
    c168->cmd_temp[1] = 0x01;
    if (buff_id == JM_V1_BOX_C168_LINKBLOCK) {
        c168->cmd_temp[2] = 0xFF;
        c168->buff_add[JM_V1_BOX_C168_LINKBLOCK] = c168->cmd_buff_len;
    } else {
        c168->cmd_temp[2] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
    }

    if (_jm_v1_box_c168_buffer_size(self) <= 1) {
        self->shared->last_error = JM_V1_BOX_C168_BUFFFLOW;
        return FALSE;
    }
    c168->cmd_temp[3] = JM_V1_BOX_C168_WR_DATA + 0x01 + c168->cmd_temp[2];
    c168->cmd_temp[0] += c168->head_password;
    c168->buff_id = buff_id;
    self->shared->is_do_now = FALSE;
    return TRUE;
}

static gboolean _jm_v1_box_c168_add_to_buff(JMV1Box *self, guint8 command_word, size_t count, const guint8 *data) {
    size_t i;
    guint8 checksum;
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;

    checksum = c168->cmd_temp[c168->cmd_temp[1] + 2];

    self->shared->next_address = c168->cmd_temp[1] + count + 1;

    if (c168->buff_id == JM_V1_BOX_C168_NULLADD) {
        // 数据块标识登记是否有申请?
        self->shared->last_error = JM_V1_BOX_C168_NOAPPLICATBUFF;
        self->shared->is_do_now = TRUE;
        return FALSE;
    }

    if (_jm_v1_box_c168_buffer_size(self) < self->shared->next_address) {
        // 检查是否有足够的空间存储?
        self->shared->last_error = JM_V1_BOX_C168_BUFFFLOW;
        self->shared->is_do_now = TRUE;
        return FALSE;
    }

    if (command_word < JM_V1_BOX_C168_RESET && command_word != JM_V1_BOX_C168_CLR_LINK
        && command_word != JM_V1_BOX_C168_DO_BAT_00 && command_word != JM_V1_BOX_C168_D0_BAT
        && command_word != JM_V1_BOX_C168_D0_BAT_FOR && command_word != JM_V1_BOX_C168_WR_DATA) {
            // 是否为缓冲区命令?
            if ((count < JM_V1_BOX_C168_CMD_DATALEN)
                || (command_word == JM_V1_BOX_C168_SEND_DATA && count < JM_V1_BOX_C168_SEND_LEN)) {
                    // 是否合法命令?
                    if (command_word == JM_V1_BOX_C168_SEND_DATA && _jm_v1_box_c168_get_box_ver(self) > 0x400) {
                        // 增加发送长命令
                        c168->cmd_temp[c168->cmd_temp[1] + 2] = JM_V1_BOX_C168_SEND_CMD;
                        checksum += JM_V1_BOX_C168_SEND_CMD;
                        c168->cmd_temp[1]++;
                        c168->cmd_temp[c168->cmd_temp[1] + 2] = command_word + count;
                        if (count > 0) {
                            c168->cmd_temp[c168->cmd_temp[1] + 2]--;
                        }
                        checksum += c168->cmd_temp[c168->cmd_temp[1] + 2];
                        c168->cmd_temp[1]++;
                        for (i = 0; i < count; i++, c168->cmd_temp[1]++) {
                            c168->cmd_temp[c168->cmd_temp[1] + 2] = data[i];
                            checksum += data[i];
                        }
                        c168->cmd_temp[c168->cmd_temp[1] + 2] = checksum + count + 2;
                        self->shared->next_address++;
                    } else {
                        c168->cmd_temp[c168->cmd_temp[1] + 2] = command_word + count;
                        if (count > 0) {
                            c168->cmd_temp[c168->cmd_temp[1] + 2]--;
                        }
                        checksum += c168->cmd_temp[c168->cmd_temp[1] + 2];
                        c168->cmd_temp[1]++;
                        for (i = 0; i < count; i++, c168->cmd_temp[1]++) {
                            c168->cmd_temp[c168->cmd_temp[1] + 2] = data[i];
                            checksum += data[i];
                        }
                        c168->cmd_temp[c168->cmd_temp[1] + 2] = checksum + count + 1;
                        self->shared->next_address++; // Rocky Add
                    }
                    return TRUE;
            }
            self->shared->last_error = JM_V1_BOX_C168_ILLIGICAL_LEN;
            self->shared->is_do_now = TRUE;
            return FALSE;
    }
    self->shared->last_error = JM_V1_BOX_C168_UNBUFF_CMD;
    self->shared->is_do_now = TRUE;
    return FALSE;
}

static gboolean _jm_v1_box_c168_end_batch(JMV1Box *self) {
    guint32 times = JM_V1_BOX_C168_REPLAYTIMES;
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);
    c168 = (JMV1BoxC168*)self->user_data;

    self->shared->is_do_now = TRUE;

    if (c168->buff_id == JM_V1_BOX_C168_NULLADD) {
        // 数据块标识登记是否有申请?
        self->shared->last_error = JM_V1_BOX_C168_NOAPPLICATBUFF;
        return FALSE;
    }

    if (c168->cmd_temp[1] == 0x01) {
        c168->buff_id = JM_V1_BOX_C168_NULLADD;
        self->shared->last_error = JM_V1_BOX_C168_NOADDDATA;
        return FALSE;
    }

    while (times--) {
        if (times == 0) {
            c168->buff_id = JM_V1_BOX_C168_NULLADD;
            return FALSE;
        }
        if (!_jm_v1_box_c168_check_idle(self) || (jm_commbox_port_write(c168->cmd_temp, c168->cmd_temp[1] + 3) != (c168->cmd_temp[1] + 3))) {
            continue;
        } else if (_jm_v1_box_c168_send_ok(self, JM_TIMER_TO_MS(20) * (c168->cmd_temp[1] + 10))) {
            break;
        }
        if (!_jm_v1_box_c168_stop_now(self, TRUE)) {
            c168->buff_id = JM_V1_BOX_C168_NULLADD;
            return FALSE;
        }
    }

    if (c168->buff_id == JM_V1_BOX_C168_LINKBLOCK) {
        c168->buff_add[JM_V1_BOX_C168_LINKBLOCK] = c168->cmd_buff_len - c168->cmd_temp[1];
    } else {
        c168->buff_add[c168->buff_id] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
        c168->buff_used[c168->buff_used_num] = c168->buff_id;
        c168->buff_used_num++;
        c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK] = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK] + c168->cmd_temp[1];
    }
    c168->buff_id = JM_V1_BOX_C168_NULLADD;
    return TRUE;
}

static gboolean _jm_v1_box_c168_send_to_box(JMV1Box *self, guint8 command_word, size_t count, const guint8 *buff) {
    g_return_val_if_fail(self != NULL, FALSE);
    if (self->shared->is_do_now) {
        return _jm_v1_box_c168_do_set(self, command_word, count, buff);
    } else {
        return _jm_v1_box_c168_add_to_buff(self, command_word, count, buff);
    }
}

static gboolean _jm_v1_box_c168_set_line_level(JMV1Box *self, guint8 value_low, guint8 value_high) {
    JMV1BoxC168 *c168 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);
    c168 = (JMV1BoxC168*)self->user_data;
    // 只有一个字节的数据，设定端口1
    c168->port[1] &= ~value_low;
    c168->port[1] |= value_high;
    return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SETPORT1, 1, c168->port + 1);
}

static gboolean _jm_v1_box_c168_set_comm_ctrl(JMV1Box *self, guint8 value_open, guint8 value_close) {
    JMV1BoxC168 *c168 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);
    c168 = (JMV1BoxC168*)self->user_data;
    // 只有一个字节的数据，设定端口2
    c168->port[2] &= ~value_open;
    c168->port[2] |= value_close;
    return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SETPORT2, 1, c168->port + 2);
}

static gboolean _jm_v1_box_c168_set_comm_line(JMV1Box *self, guint8 send_line, guint8 recv_line) {
    JMV1BoxC168 *c168 = NULL;
    g_return_val_if_fail(self != NULL, FALSE);
    c168 = (JMV1BoxC168*)self->user_data;
    // 只有一个字节的数据，设定端口0
    if (send_line > 7) {
        send_line = 0x0F;
    }
    if (recv_line > 7) {
        recv_line = 0x0F;
    }
    c168->port[0] = send_line + recv_line << 4;
    return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SETPORT0, 1, c168->port);
}

static gboolean _jm_v1_box_c168_turn_over_one_by_one(JMV1Box *self) {
    // 将原有的接受一个发送一个的标志翻转
    return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SET_ONEBYONE, 0, NULL);
}

static gboolean _jm_v1_box_c168_set_echo_data(JMV1Box *self, guint8 *echo_buff, size_t echo_len) {
    g_return_val_if_fail(self != NULL, FALSE);
    if (echo_buff == NULL || echo_len == 0 || echo_len > 4) {
        self->shared->last_error = JM_V1_BOX_C168_ILLIGICAL_LEN;
        return FALSE;
    }
    if (self->shared->is_do_now) {
        guint8 receive_buff[6];
        size_t i;
        if (!_jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_ECHO, echo_len, echo_buff)
            || (_jm_v1_box_c168_read_data(self, receive_buff, echo_len, JM_TIMER_TO_MS(100)) != echo_len)) {
                return FALSE;
        }
        for (i = 0; i < echo_len; i++) {
            if (receive_buff[i] != echo_buff[i]) {
                self->shared->last_error = JM_V1_BOX_C168_CHECKSUM_ERROR;
                return FALSE;
            }
        }
        return _jm_v1_box_c168_check_result(self, JM_TIMER_TO_MS(100));
    }
    return _jm_v1_box_c168_add_to_buff(self, JM_V1_BOX_C168_ECHO, echo_len, echo_buff);
}

static gboolean _jm_v1_box_c168_keep_link(JMV1Box *self, gboolean is_run_link) {
    if (is_run_link) {
        return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_RUNLINK, 0, NULL);
    }
    return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_STOPLINK, 0, NULL);
}

static gboolean _jm_v1_box_c168_set_comm_link(JMV1Box *self, guint8 ctrl_word1, guint8 ctrl_word2, guint8 ctrl_word3) {
    guint8 ctrl_word[3]; // 通讯控制字3
    guint8 mode_control = ctrl_word1 & 0xE0;
    size_t length = 3;

    g_return_val_if_fail(self != NULL, FALSE);

    ctrl_word[0] = ctrl_word1;

    if ((ctrl_word1 & 0x04) != 0) {
        self->shared->is_db_20 = TRUE;
    } else {
        self->shared->is_db_20 = FALSE;
    }
    
    if (mode_control == JM_V1_BOX_C168_SET_VPW || mode_control == JM_V1_BOX_C168_SET_PWM) {
        return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SETTING, 1, ctrl_word);
    }

    ctrl_word[1] = ctrl_word2;
    ctrl_word[2] = ctrl_word3;
    if (ctrl_word3 == 0) {
        length--;
        if (ctrl_word2 == 0) {
            length--;
        }
    }
    if (mode_control == JM_V1_BOX_C168_EXRS_232 && length < 2) {
        self->shared->last_error = JM_V1_BOX_C168_UNSET_EXRSBIT;
        return FALSE;
    }

    return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SETTING, length, ctrl_word);
}

static gboolean _jm_v1_box_c168_set_comm_baud(JMV1Box *self, gdouble baud) {
    guint8 baud_time[2];
    gdouble instruct_num;
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);
    c168 = (JMV1BoxC168*)self->user_data;

    instruct_num = ((1000000.0 / (c168->time_unit)) * 1000000) / baud;

    if (self->shared->is_db_20) {
        instruct_num /= 20;
    }

    instruct_num += 0.5;
    if (instruct_num > 65535 || instruct_num < 10) {
        self->shared->last_error = JM_V1_BOX_C168_COMMBAUD_OUT;
        return FALSE;
    }
    baud_time[0] = JM_HIGH_BYTE(instruct_num);
    baud_time[1] = JM_LOW_BYTE(instruct_num);
    if (baud_time[0] == 0) {
        return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SETBAUD, 1, baud_time + 1);
    } else {
        return _jm_v1_box_c168_send_to_box(self, JM_V1_BOX_C168_SETBAUD, 2, baud_time);
    }
}

static void _jm_v1_box_c168_get_link_time(JMV1Box *self, guint8 type, guint32 time) {
    if (type == JM_V1_BOX_C168_SETBYTETIME) {
        self->shared->req_byte_to_byte = time;
    } else if (type == JM_V1_BOX_C168_SETWAITTIME) {
        self->shared->req_wait_time = time;
    } else if (type == JM_V1_BOX_C168_SETRECBBOUT) {
        self->shared->res_byte_to_byte = time;
    } else if (type == JM_V1_BOX_C168_SETRECFROUT) {
        self->shared->res_wait_time = time;
    }
}

static gboolean _jm_v1_box_c168_set_comm_time(JMV1Box *self, guint8 type, guint32 time) {
    guint8 time_buff[2];
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);
    c168 = (JMV1BoxC168*)self->user_data;

    _jm_v1_box_c168_get_link_time(self, type, time);

    if (type == JM_V1_BOX_C168_SETVPWSTART || type == JM_V1_BOX_C168_SETVPWRECS) {
        if (JM_V1_BOX_C168_SETVPWRECS == type) {
            time = (time * 2) / 3;
        }
        type = type + (JM_V1_BOX_C168_SETBYTETIME & 0xF0);
        time = time / ((c168->time_unit / 1000000.0));
    } else {
        time = ((time / c168->time_base_db) / (c168->time_unit / 1000000.0));
    }

    if (time > 65535) {
        self->shared->last_error = JM_V1_BOX_C168_COMMTIME_OUT;
        return FALSE;
    }

    if (type == JM_V1_BOX_C168_SETBYTETIME
        || type == JM_V1_BOX_C168_SETWAITTIME
        || type == JM_V1_BOX_C168_SETRECBBOUT
        || type == JM_V1_BOX_C168_SETRECFROUT
        || type == JM_V1_BOX_C168_SETLINKTIME) {
            time_buff[0] = JM_HIGH_BYTE(time);
            time_buff[1] = JM_LOW_BYTE(time);
            if (time_buff[0] == 0) {
                return _jm_v1_box_c168_send_to_box(self, type, 1, time_buff + 1);
            }
            return _jm_v1_box_c168_send_to_box(self, type, 2, time_buff);
    }
    self->shared->last_error = JM_V1_BOX_C168_UNDEFINE_CMD;
    return FALSE;
}

static gboolean _jm_v1_box_c168_run_receive(JMV1Box *self, guint8 type) {
    g_return_val_if_fail(self != NULL, FALSE);

    if (type == JM_V1_BOX_C168_GET_PORT1) {
        self->shared->is_db_20 = FALSE;
    }

    if (type == JM_V1_BOX_C168_GET_PORT1 || type == JM_V1_BOX_C168_SET55_BAUD
        || (type >= JM_V1_BOX_C168_REC_FR && type <= JM_V1_BOX_C168_RECEIVE)) {
            if (self->shared->is_do_now) {
                return _jm_v1_box_c168_commbox_do(self, type, 0, NULL);
            }
            return _jm_v1_box_c168_add_to_buff(self, type, 0, NULL);
    }
    self->shared->last_error = JM_V1_BOX_C168_UNDEFINE_CMD;
    return FALSE;
}

static guint8 _jm_v1_box_c168_get_abs_add(JMV1Box *self, guint8 buff_id, guint8 add) {
    size_t length = 0;
    guint8 start_add = 0;
    JMV1BoxC168 *c168 = NULL;
    
    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;

    if (c168->buff_id != buff_id) {
        if (c168->buff_add[buff_id] == JM_V1_BOX_C168_NULLADD) {
            self->shared->last_error = JM_V1_BOX_C168_NOUSED_BUFF;
            return 0;
        }

        if (buff_id == JM_V1_BOX_C168_LINKBLOCK) {
            length = c168->cmd_buff_len - c168->buff_add[JM_V1_BOX_C168_LINKBLOCK];
        } else {
            size_t i;
            for (i = 0; i < c168->buff_used_num; i++) {
                if (c168->buff_used[i] == buff_id) {
                    break;
                }
            }
            if (i == (c168->buff_used_num - 1)) {
                length = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK] - c168->buff_add[buff_id];
            } else {
                length = c168->buff_add[buff_id + 1] - c168->buff_add[buff_id];
            }
        }
        start_add = c168->buff_add[buff_id];
    } else {
        length = c168->buff_add[JM_V1_BOX_C168_LINKBLOCK] - c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
        start_add = c168->buff_add[JM_V1_BOX_C168_SWAPBLOCK];
    }

    if (add < length) {
        return add + start_add;
    }

    self->shared->last_error = JM_V1_BOX_C168_OUTADDINBUFF;
    return 0;
}

static gboolean _jm_v1_box_c168_update_buff(JMV1Box *self, guint8 type, guint8 *buffer) {
    guint8 cmd_temp[4];
    guint8 ret;

    g_return_val_if_fail(self != NULL, FALSE);

    self->shared->last_error = 0;
    ret = _jm_v1_box_c168_get_abs_add(self, buffer[0], buffer[1]);
    if (ret == 0) {
        return FALSE;
    }
    cmd_temp[0] = ret;

    if ((type == JM_V1_BOX_C168_INVERT_DATA) || (type == JM_V1_BOX_C168_DEC_DATA) || (type == JM_V1_BOX_C168_INC_DATA)) {
    } else if ((type == JM_V1_BOX_C168_UPDATE_1BYTE) || (type == JM_V1_BOX_C168_SUB_BYTE)) {
        cmd_temp[1] = buffer[2];
    } else if (type == JM_V1_BOX_C168_INC_2DATA) {
        ret = _jm_v1_box_c168_get_abs_add(self, buffer[2], buffer[3]);
        if (ret == 0) {
            return FALSE;
        }
        cmd_temp[1] = ret;
    } else if ((type == JM_V1_BOX_C168_COPY_DATA) || (type == JM_V1_BOX_C168_ADD_1BYTE)) {
        ret = _jm_v1_box_c168_get_abs_add(self, buffer[2], buffer[3]);
        if (ret == 0) {
            return FALSE;
        }
        cmd_temp[1] = ret;
        cmd_temp[2] = buffer[4];
    } else if ((type == JM_V1_BOX_C168_UPDATE_2BYTE) || (type == JM_V1_BOX_C168_ADD_2BYTE)) {
        ret = _jm_v1_box_c168_get_abs_add(self, buffer[3], buffer[4]);
        if (ret == 0) {
            return FALSE;
        }
        cmd_temp[1] = buffer[2];
        cmd_temp[2] = ret;
        cmd_temp[3] = buffer[5];

    } else if ((type == JM_V1_BOX_C168_ADD_DATA) || (type == JM_V1_BOX_C168_SUB_DATA)) {
        ret = _jm_v1_box_c168_get_abs_add(self, buffer[2], buffer[3]);
        if (ret == 0) {
            return FALSE;
        }
        cmd_temp[1] = ret;
        ret = _jm_v1_box_c168_get_abs_add(self, buffer[4], buffer[5]);
        if (ret == 0) {
            return FALSE;
        }
        cmd_temp[2] = ret;

    } else {
        self->shared->last_error = JM_V1_BOX_C168_UNDEFINE_CMD;
        return FALSE;

    }
    return _jm_v1_box_c168_send_to_box(self, type - type % 4, type % 4 + 1, cmd_temp);
}

static gboolean _jm_v1_box_c168_commbox_delay(JMV1Box *self, guint32 time) {
    guint8 time_buff[2];
    guint8 delay_word = JM_V1_BOX_C168_DELAYSHORT;
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;

    time = time / (c168->time_unit / 1000000.0);
    if (time == 0) {
        self->shared->last_error = JM_V1_BOX_C168_SETTIME_ERROR;
        return FALSE;
    }
    if (time > 65535) {
        time = time / c168->time_base_db;
        if (time > 65535) {
            time = time / c168->time_base_db;
            if (time > 65535) {
                self->shared->last_error = JM_V1_BOX_C168_COMMTIME_OUT;
                return FALSE;
            }
            delay_word = JM_V1_BOX_C168_DELAYLONG;
        } else {
            delay_word = JM_V1_BOX_C168_DELAYTIME;
        }
    }
    time_buff[0] = JM_HIGH_BYTE(time);
    time_buff[1] = JM_LOW_BYTE(time);
    if (time_buff[0] == 0) {
        if (self->shared->is_do_now) {
            return _jm_v1_box_c168_commbox_do(self, delay_word, 1, time_buff + 1);
        }
        return _jm_v1_box_c168_add_to_buff(self, delay_word, 1, time_buff + 1);
    }
    if (self->shared->is_do_now) {
        return _jm_v1_box_c168_commbox_do(self, delay_word, 2, time_buff);
    }
    return _jm_v1_box_c168_add_to_buff(self, delay_word, 2, time_buff);
}

static gboolean _jm_v1_box_c168_send_out_data(JMV1Box *self, const guint8 *buffer, size_t count) {

    g_return_val_if_fail(self != NULL, FALSE);

    if (count == 0) {
        self->shared->last_error = JM_V1_BOX_C168_ILLIGICAL_LEN;
        return FALSE;
    }
    if (self->shared->is_do_now) {
        return _jm_v1_box_c168_commbox_do(self, JM_V1_BOX_C168_SEND_DATA, count, buffer);
    } else {
        return _jm_v1_box_c168_add_to_buff(self, JM_V1_BOX_C168_SEND_DATA, count, buffer);
    }
}

static gboolean _jm_v1_box_c168_run_batch(JMV1Box *self, guint8 *buff_id, size_t count, gboolean is_execute_many) {
    guint8 command_word = JM_V1_BOX_C168_D0_BAT;
    size_t i;
    JMV1BoxC168 *c168 = NULL;

    g_return_val_if_fail(self != NULL, FALSE);

    c168 = (JMV1BoxC168*)self->user_data;

    for (i = 0; i < count; i++) {
        if (c168->buff_add[buff_id[i]] == JM_V1_BOX_C168_NULLADD) {
            self->shared->last_error = JM_V1_BOX_C168_NOUSED_BUFF;
            return FALSE;
        }
    }
    if (is_execute_many) {
        command_word = JM_V1_BOX_C168_D0_BAT_FOR;
    }
    if (command_word == JM_V1_BOX_C168_D0_BAT && buff_id[0] == c168->buff_used[0]) {
        command_word = JM_V1_BOX_C168_DO_BAT_00;
        return _jm_v1_box_c168_commbox_do(self, command_word, 0, NULL);
    }

    return _jm_v1_box_c168_commbox_do(self, command_word, count, buff_id);
}

static size_t _jm_v1_box_c168_read_bytes(JMV1Box *self, guint8 *buff, size_t count) {
    g_return_val_if_fail(self != NULL, FALSE);
    return _jm_v1_box_c168_read_data(self, buff, count, self->shared->res_wait_time);
}

static gint32 _jm_v1_box_c168_serial_port_baud(JMV1Box *self) {
    g_return_val_if_fail(self != NULL, 0);
    return ((JMV1BoxC168*)self->user_data)->serial_port_baud;
}

static guint8 _jm_v1_box_c168_serial_port_databits(JMV1Box *self) {
    return 8;
}

static gint32 _jm_v1_box_c168_serial_port_parity(JMV1Box *self) {
    return JM_SP_PAR_NONE;
}

static gint32 _jm_v1_box_c168_serial_port_stopbits(JMV1Box *self) {
    return JM_SP_SB_ONE;
}

static gint32 _jm_v1_box_c168_serial_port_flow_control(JMV1Box *self) {
    return JM_SP_FC_NONE;
}

JMV1Box* jm_v1_box_c168_new(JMV1Shared *shared) {
    JMV1Box *obj = jm_v1_box_new(shared);
    JMV1BoxC168 *c168 = (JMV1BoxC168*)g_malloc(sizeof(JMV1BoxC168));

    c168->serial_port_baud = 9600;
    c168->buff_used_num = 0;
    c168->time_unit = 0;
    c168->time_base_db = 0;
    c168->time_extern_db = 0;
    c168->cmd_buff_len = 0;
    c168->is_initialize = TRUE;
    c168->buff_id = 0;
    c168->head_password = 0;
    memset(c168->buff_add, 0, JM_V1_BOX_C168_MAXIM_BLOCK + 2);
    memset(c168->buff_used, 0, JM_V1_BOX_C168_MAXIM_BLOCK);
    memset(c168->version, 0, JM_V1_BOX_C168_VERSIONLEN);
    memset(c168->box_id, 0, JM_V1_BOX_C168_COMMBOXIDLEN);
    memset(c168->port, 0, JM_V1_BOX_C168_COMMBOXPORTNUM);
    memset(c168->cmd_temp, 0, sizeof(c168->cmd_temp));

    obj->user_data = c168;
    obj->check_result = _jm_v1_box_c168_check_result;
    obj->check_serial_port_change_config = _jm_v1_box_c168_check_serial_port_change_config;
    obj->close_comm = _jm_v1_box_c168_close_comm;
    obj->commbox_delay = _jm_v1_box_c168_commbox_delay;
    obj->del_batch = _jm_v1_box_c168_del_batch;
    obj->end_batch = _jm_v1_box_c168_end_batch;
    obj->get_abs_add = _jm_v1_box_c168_get_abs_add;
    obj->keep_link = _jm_v1_box_c168_keep_link;
    obj->new_batch = _jm_v1_box_c168_new_batch;
    obj->open_comm = _jm_v1_box_c168_open_comm;
    obj->read_bytes = _jm_v1_box_c168_read_bytes;
    obj->read_data = _jm_v1_box_c168_read_data;
    obj->run_batch = _jm_v1_box_c168_run_batch;
    obj->run_receive = _jm_v1_box_c168_run_receive;
    obj->send_out_data = _jm_v1_box_c168_send_out_data;
    obj->serial_port_change_config = _jm_v1_box_c168_serial_port_change_config;
    obj->serial_port_databits = _jm_v1_box_c168_serial_port_databits;
    obj->serial_port_flow_control = _jm_v1_box_c168_serial_port_flow_control;
    obj->serial_port_parity = _jm_v1_box_c168_serial_port_parity;
    obj->serial_port_baud = _jm_v1_box_c168_serial_port_baud;
    obj->serial_port_stopbits = _jm_v1_box_c168_serial_port_stopbits;
    obj->set_comm_baud = _jm_v1_box_c168_set_comm_baud;
    obj->set_comm_time = _jm_v1_box_c168_set_comm_time;
    obj->set_comm_ctrl = _jm_v1_box_c168_set_comm_ctrl;
    obj->set_comm_line = _jm_v1_box_c168_set_comm_line;
    obj->set_comm_link = _jm_v1_box_c168_set_comm_link;
    obj->set_line_level = _jm_v1_box_c168_set_line_level;
    obj->stop_now = _jm_v1_box_c168_stop_now;
    obj->turn_over_one_by_one = _jm_v1_box_c168_turn_over_one_by_one;
    obj->update_buff = _jm_v1_box_c168_update_buff;

    obj->box_type = JM_V1_BOX_C168;

    return obj;
}

void jm_v1_box_c168_free(JMV1Box *self) {
    g_return_if_fail(self != NULL);

    g_free(self->user_data);
    self->user_data = NULL;

    jm_v1_box_free(self);
}
