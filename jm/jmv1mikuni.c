#include <stdlib.h>
#include "jmv1mikuni.h"
#include "jmerror.h"
#include "jmtimer.h"
#include "jmutils.h"
#include "jmv1default.h"

typedef struct _JMV1Mikuni JMV1Mikuni;

struct _JMV1Mikuni {
    JMV1Box *box;
};

static gint32 _jm_v1_mikuni_init(JMMikuni *self) {
    JMV1Mikuni *v1 = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;
    guint8 value_open;
    guint8 value_close;
    guint8 send_line;
    guint8 recv_line;
    guint8 ctrl_word1;
    guint8 ctrl_word2;
    guint8 ctrl_word3;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Mikuni *)self->user_data;
    box = v1->box;
    shared = box->shared;

    value_open = JM_V1_BOX_C(box, PWC) | JM_V1_BOX_C(box, RZFC) | JM_V1_BOX_C(box, CK) | JM_V1_BOX_C(box, REFC);
    value_close = JM_V1_BOX_C(box, SET_NULL);
    send_line = JM_V1_BOX_C(box, SK_NO);
    recv_line = JM_V1_BOX_C(box, RK1);
    ctrl_word1 = JM_V1_BOX_C(box, RS_232) | JM_V1_BOX_C(box, BIT9_MARK) | JM_V1_BOX_C(box, SEL_SL) | JM_V1_BOX_C(box, UN_DB20);
    ctrl_word2 = 0xFF;
    ctrl_word3 = 2;

    if (!box->set_comm_ctrl(box, value_open, value_close) ||
        !box->set_comm_line(box, send_line, recv_line) ||
        !box->set_comm_link(box, ctrl_word1, ctrl_word2, ctrl_word3) ||
        !box->set_comm_baud(box, 19200) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETBYTETIME), JM_TIMER_TO_MS(100)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETWAITTIME), JM_TIMER_TO_MS(1)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECBBOUT), JM_TIMER_TO_MS(400)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETRECFROUT), JM_TIMER_TO_MS(500)) ||
        !box->set_comm_time(box, JM_V1_BOX_C(box, SETLINKTIME), JM_TIMER_TO_MS(500))) {
            return JM_ERROR_GENERIC;
    }

    g_usleep(JM_TIMER_TO_SEC(1));
    return JM_ERROR_SUCCESS;
}

void _jm_v1_mikuni_finish_execute(JMMikuni *self, gboolean is_finish) {
    JMV1Box *box = NULL;
    g_return_if_fail(self != NULL);

    box = ((JMV1Mikuni*)self->user_data)->box;
    if (is_finish) {
        box->stop_now(box, TRUE);
        box->del_batch(box, box->shared->buff_id);
        box->check_result(box, JM_TIMER_TO_MS(500));
    }
}

static size_t _jm_v1_mikuni_send_one_frame(JMMikuni *self, const guint8 *data, size_t count, gboolean need_recv) {
    JMV1Mikuni *mikuni = NULL;
    JMV1Box *box = NULL;
    JMV1Shared *shared = NULL;

    g_return_val_if_fail(self != NULL, 0);

    mikuni = (JMV1Mikuni *)self->user_data;
    box = mikuni->box;
    shared = box->shared;

    JM_V1_DEFAULT_SEND_ONE_FRAME(shared, box, self, data, count, need_recv, mikuni);
}

size_t jm_v1_mikuni_send_one_frame(JMMikuni *self, const guint8 *data, size_t count) {
    return _jm_v1_mikuni_send_one_frame(self, data, count, FALSE);
}

size_t jm_v1_mikuni_send_frames(JMMikuni *self, const guint8 *data, size_t count) {
    return jm_v1_mikuni_send_one_frame(self, data, count);
}

static size_t _jm_v1_mikuni_read_one_frame(JMMikuni *self, guint8 *data, gboolean is_finish) {
    size_t temp_length = 0;
    guint8 temp[256] = {0};
    guint8 before = 0;
    guint8 *p = temp;
    JMV1Box *box = NULL;

    g_return_val_if_fail(self != NULL, 0);

    box = ((JMV1Mikuni*)self->user_data)->box;

    while (box->read_bytes(box, p++, 1) == 1) {
        temp_length++;
        if (before == 0x0D && (*p) == 0x0A) {
            break;
        }
        before = *p;
    }

    if (before == 0x0D && *p == 0x0A) {
        // break normal
        temp_length = jm_mikuni_unpack(self, temp, temp_length, data);
    } else {
        temp_length = 0;
    }
    _jm_v1_mikuni_finish_execute(self, is_finish);
    return temp_length;
}

size_t jm_v1_mikuni_read_one_frame(JMMikuni *self, guint8 *data) {
    return _jm_v1_mikuni_read_one_frame(self, data, TRUE);
}

size_t jm_v1_mikuni_read_frames(JMMikuni *self, guint8 *data) {
    return jm_v1_mikuni_read_one_frame(self, data);
}

gint32 jm_v1_mikuni_set_keep_link(JMMikuni *self, const guint8 *data, size_t count) {
    guint8 buff[256];
    size_t length;
    JMV1Box *box = NULL;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    box = ((JMV1Mikuni*)self->user_data)->box;

    length = jm_mikuni_pack(self, data, count, buff);

    if (length <= 0)
        return JM_ERROR_GENERIC;

    JM_V1_DEFAULT_SET_KEEP_LINK(box, buff, length);
}

JMMikuni* jm_v1_mikuni_new(JMV1Box *box) {
    JMMikuni *obj = jm_mikuni_new();
    JMV1Mikuni *v1 = (JMV1Mikuni*)g_malloc(sizeof(JMV1Mikuni));

    v1->box = box;

    obj->user_data = v1;
    obj->init = _jm_v1_mikuni_init;

    return obj;
}

void jm_v1_mikuni_free(JMMikuni *self) {
    g_return_if_fail(self != NULL);
    g_free(self->user_data);
    jm_mikuni_free(self);
}