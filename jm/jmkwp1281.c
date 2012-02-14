#include <string.h>
#include "jmkwp1281.h"
#include "jmutils.h"
#include "jmerror.h"

static guint8 _jm_kwp1281_frame_counter_increment(JMKWP1281 *self) {
    return ++(self->frame_counter);
}

static size_t _jm_kwp1281_pack(JMKWP1281 *self, const guint8 *src, size_t count, guint8 *tar) {
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(count > 0, 0);

    tar[0] = JM_LOW_BYTE(count + 20);
    tar[1] = _jm_kwp1281_frame_counter_increment(self);

    memcpy(tar + 2, src, count);
    tar[count + 2] = JM_KWP1281_FRAME_END;
    return count + 3;
}

static size_t _jm_kwp1281_unpack(JMKWP1281 *self, const guint8 *src, size_t count, guint8 *tar) {
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail((gint32)(count - 2) > 0, 0);

    memcpy(tar, src + 1, count - 2);
    return count - 2;
}

JMKWP1281* jm_kwp1281_new(void) {
    JMKWP1281 *obj = (JMKWP1281*)g_malloc(sizeof(JMKWP1281));

    obj->frame_counter = 0;
    obj->addr_init = NULL;
    obj->set_lines = NULL;
    obj->pack = _jm_kwp1281_pack;
    obj->unpack = _jm_kwp1281_unpack;
    obj->user_data = NULL;

    return obj;
}

void jm_kwp1281_free(JMKWP1281 *self) {
    g_return_if_fail(self != NULL);

    g_free(self);
}

size_t jm_kwp1281_pack(JMKWP1281 *self, const guint8 *src, size_t count, guint8 *tar) {
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->pack != NULL, 0);

    return self->pack(self, src, count, tar);
}

size_t jm_kwp1281_unpack(JMKWP1281 *self, const guint8 *src, size_t count, guint8 *tar) {
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->unpack != NULL, 0);

    return self->unpack(self, src, count, tar);
}

gint32 jm_kwp1281_addr_init(JMKWP1281 *self, guint8 addr_code) {
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->addr_init != NULL, JM_ERROR_GENERIC);

    return self->addr_init(self, addr_code);
}

gint32 jm_kwp1281_set_lines(JMKWP1281 *self, gint32 com_line, gboolean l_line) {
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_lines != NULL, JM_ERROR_GENERIC);

    return self->set_lines(self, com_line, l_line);
}
