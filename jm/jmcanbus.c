#include <string.h>
#include "jmcanbus.h"
#include "jmutils.h"
#include "jmerror.h"

gint32 _jm_canbus_set_options(JMCanbus *self, 
    gint32 id, 
    JMCanbusBaud baud, 
    JMCanbusIDMode id_mode, 
    JMCanbusFilterMask mask, 
    JMCanbusFrameType frame) {
    g_return_val_if_fail(self != NULL, FALSE);

    self->baud = baud;
    self->id_mode = id_mode;
    self->filter_mask = mask;
    self->frame_type = frame;
    self->target_id = id;
    return JM_ERROR_SUCCESS;
}

static size_t _jm_canbus_pack(JMCanbus *self, const guint8 *src, size_t count, guint8 *tar) {
    g_return_val_if_fail(self != NULL, 0);

    if (count > 8 || count <= 0) {
        return 0;
    }

    if (self->id_mode == JM_CANBUS_ID_MODE_STD) {
        tar[1] = JM_HIGH_BYTE(JM_LOW_WORD(self->target_id));
        tar[2] = JM_LOW_BYTE(JM_LOW_WORD(self->target_id));
        if (self->frame_type == JM_CANBUS_FRAME_TYPE_DATA) {
            tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA);
        } else {
            tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_REMOTE);
        }
        memcpy(tar + 3, src, count);
        return count + 3;
    }
    if (self->id_mode == JM_CANBUS_ID_MODE_EXT) {
        tar[1] = JM_HIGH_BYTE(JM_HIGH_WORD(self->target_id));
        tar[2] = JM_LOW_BYTE(JM_HIGH_WORD(self->target_id));
        tar[3] = JM_HIGH_BYTE(JM_LOW_WORD(self->target_id));
        tar[4] = JM_LOW_BYTE(JM_LOW_WORD(self->target_id));
        if (self->frame_type == JM_CANBUS_FRAME_TYPE_DATA) {
            tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA);
        } else {
            tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE);
        }
        memcpy(tar + 5, src, count);
        return count + 5;
    }
    return 0;
}

static size_t _jm_canbus_unpack(JMCanbus *self, const guint8 *src, size_t count, guint8 *tar) {
    guint32 mode;
    size_t length = 0;

    g_return_val_if_fail(self != NULL, 0);

    if (count <= 0) {
        return 0;
    }

    mode = (src[0] & (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE));
    if (mode == (JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA)) {
        length = src[0] & 0x0F;
        if (length != count - 3) {
            return 0;
        }
        memcpy(tar, src + 3, length);
        return length;
    }

    if (mode == (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA)) {
        length = src[0] & 0x0F;
        if (length != count - 5) {
            return 0;
        }
        memcpy(tar, src + 5, length);
    }
    return length;
}

JMCanbus* jm_canbus_new(void) {
    JMCanbus *obj = (JMCanbus*)g_malloc(sizeof(JMCanbus));
    obj->target_id = 0;
    obj->baud = JM_CANBUS_B500K;
    obj->id_mode = JM_CANBUS_ID_MODE_STD;
    obj->filter_mask = JM_CANBUS_FILTER_MASK_DISABLE;
    obj->frame_type = JM_CANBUS_FRAME_TYPE_DATA;
    obj->high = 0;
    obj->low = 0;
    obj->flow_control[0] = 0x30;
    obj->flow_control[1] = 0x00;
    obj->flow_control[2] = 0x00;
    obj->flow_control[3] = 0x00;
    obj->flow_control[4] = 0x00;
    obj->flow_control[5] = 0x00;
    obj->flow_control[6] = 0x00;
    obj->flow_control[7] = 0x00;
    obj->id_array = g_ptr_array_new();
    obj->user_data = NULL;

    obj->pack = _jm_canbus_pack;
    obj->unpack = _jm_canbus_unpack;
    obj->set_options = _jm_canbus_set_options;

    obj->set_lines = NULL;
    obj->set_filter = NULL;

    return obj;
}

void jm_canbus_free(JMCanbus *self) {
    g_return_if_fail(self != NULL);

    g_ptr_array_free(self->id_array, FALSE);

    g_free(self);
}

size_t jm_canbus_pack(JMCanbus *self, const guint8 *src, size_t count, guint8 *tar) {
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->pack != NULL, 0);
    return self->pack(self, src, count, tar);
}

size_t jm_canbus_unpack(JMCanbus *self, const guint8 *src, size_t count, guint8 *tar) {
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->unpack != NULL, 0);
    return self->unpack(self, src, count, tar);
}

gint32 jm_canbus_set_lines(JMCanbus *self, gint32 high, gint32 low) {
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_lines != NULL, JM_ERROR_GENERIC);
    return self->set_lines(self, high, low);
}

gint32 jm_canbus_set_filter(JMCanbus *self, const gint32 *id_array, size_t count) {
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_filter != NULL, JM_ERROR_GENERIC);
    return self->set_filter(self, id_array, count);
}

gint32 jm_canbus_set_options(JMCanbus *self, gint32 id, JMCanbusBaud baud, JMCanbusIDMode id_mode, JMCanbusFilterMask mask, JMCanbusFrameType frame) {
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_options != NULL, JM_ERROR_GENERIC);
    return self->set_options(self, id, baud, id_mode, mask, frame);
}
