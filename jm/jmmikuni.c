#include <string.h>
#include "jmmikuni.h"
#include "jmerror.h"

static size_t _jm_mikuni_pack(JMMikuni *self, const guint8 *src, 
    size_t count, guint8 *tar)
{
    g_return_val_if_fail(self != NULL, 0);

    if (count <= 0)
    {
        return 0;
    }

    tar[0] = JM_MIKUNI_HEAD_FORMAT;
    memcpy(tar + 1, src, count);
    tar[count + 1] = 0x0D;
    tar[count + 2] = 0x0A;
    return count + 3;
}

static size_t _jm_mikuni_unpack(JMMikuni *self, const guint8 *src, 
    size_t count, guint8 *tar)
{
    g_return_val_if_fail(self != NULL, 0);

    if (count <= 0)
    {
        return 0;
    }

    memcpy(tar, src + 1, count - 3);
    return count - 3;
}

JMMikuni* jm_mikuni_new(void)
{
    JMMikuni *obj = (JMMikuni*)g_malloc(sizeof(JMMikuni));

    obj->pack = _jm_mikuni_pack;
    obj->unpack = _jm_mikuni_unpack;
    obj->user_data = NULL;

    return obj;
}

void jm_mikuni_free(JMMikuni *self)
{
    g_return_if_fail(self != NULL);

    g_free(self);
}

size_t jm_mikuni_pack(JMMikuni *self, const guint8 *src, size_t count, 
    guint8 *tar)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->pack != NULL, 0);

    return self->pack(self, src, count, tar);
}

size_t jm_mikuni_unpack(JMMikuni *self, const guint8 *src, size_t count, 
    guint8 *tar)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->unpack != NULL, 0);

    return self->unpack(self, src, count, tar);
}

gint32 jm_mikuni_init(JMMikuni *self)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->init != NULL, JM_ERROR_GENERIC);

    return self->init(self);
}