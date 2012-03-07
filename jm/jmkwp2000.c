#include <string.h>
#include "jmkwp2000.h"
#include "jmerror.h"
#include "jmutils.h"

static gint32 _jm_kwp2000_set_options(JMKWP2000 *self, JMKWPMode msg_mode, 
    JMKWPMode link_mode, gint32 baud)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    self->msg_mode = msg_mode;
    self->link_mode = link_mode;
    self->mode = msg_mode;
    self->baud = baud;

    return JM_ERROR_SUCCESS;
}

static gint32 _jm_kwp2000_set_address(JMKWP2000 *self, 
    guint32 target_address, guint32 source_address)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    self->target_address = target_address;
    self->source_address = source_address;

    return JM_ERROR_SUCCESS;
}

static size_t _jm_kwp2000_pack(JMKWP2000 *self, const guint8 *src, 
    size_t count, guint8 *tar)
{
    size_t pos = 0;
    guint8 checksum = 0;
    size_t i = 0;
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(count > 0, 0);

    if (self->mode == JM_KWP8X)
    {
        tar[pos++] = JM_LOW_BYTE(0x80 | count);
        tar[pos++] = JM_LOW_BYTE(self->target_address);
        tar[pos++] = JM_LOW_BYTE(self->source_address);
        memcpy(tar + pos, src, count);
        pos += count;
    }
    else if (self->mode == JM_KWPCX)
    {
        tar[pos++] = JM_LOW_BYTE(0xC0 | count);
        tar[pos++] = JM_LOW_BYTE(self->target_address);
        tar[pos++] = JM_LOW_BYTE(self->source_address);
        memcpy(tar + pos, src, count);
        pos += count;
    }
    else if (self->mode == JM_KWP80)
    {
        tar[pos++] = JM_LOW_BYTE(0x80);
        tar[pos++] = JM_LOW_BYTE(self->target_address);
        tar[pos++] = JM_LOW_BYTE(self->source_address);
        tar[pos++] = JM_LOW_BYTE(count);
        memcpy(tar + pos, src, count);
        pos += count;
    }
    else if (self->mode == JM_KWP00)
    {
        tar[pos++] = 0x00;
        tar[pos++] = JM_LOW_BYTE(count);
        memcpy(tar + pos, src, count);
        pos += count;
    }
    else if (self->mode == JM_KWPXX)
    {
        tar[pos++] = JM_LOW_BYTE(count);
        memcpy(tar + pos, src, count);
        pos += count;
    }

    for (i = 0; i < pos; i++)
    {
        checksum += tar[i];
    }
    tar[pos++] = checksum;
    return pos;

}

static size_t _jm_kwp2000_unpack(JMKWP2000 *self, const guint8 *src, 
    size_t count, guint8 *tar)
{
    size_t length = 0;
    g_return_val_if_fail(self != 0, 0);
    g_return_val_if_fail(count > 0, 0);

    if (src[0] > 0x80)
    {
        length = src[0] - 0x80;
        if (src[1] != self->source_address)
        {
            return 0;
        }
        if (length != (count - JM_KWP8X_HEADER_LENGTH - 
            JM_KWP_CHECKSUM_LENGTH))
        {
            length = src[0] - 0xC0; /* For KWPCX */
            if (length != (count - JM_KWPCX_HEADER_LENGTH - 
                JM_KWP_CHECKSUM_LENGTH))
            {
                return 0;
            }
        }
        memcpy(tar, src + JM_KWP8X_HEADER_LENGTH, length);
    }
    else if (src[0] == 0x80)
    {
        length = src[0];
        if (src[1] != self->source_address)
        {
            return 0;
        }

        if (length != (count - JM_KWP80_HEADER_LENGTH - 
            JM_KWP_CHECKSUM_LENGTH))
        {
            return 0;
        }
        memcpy(tar, src + JM_KWP80_HEADER_LENGTH, length);
    }
    else if (src[0] == 0x00)
    {
        length = src[1];
        if (length != (count - JM_KWP00_HEADER_LENGTH - 
            JM_KWP_CHECKSUM_LENGTH))
        {
            return 0;
        }
        memcpy(tar, src + JM_KWP00_HEADER_LENGTH, length);
    }
    else
    {
        length = src[0];
        if (length != (count - JM_KWP00_HEADER_LENGTH - 
            JM_KWP_CHECKSUM_LENGTH))
        {
            return 0;
        }
        memcpy(tar, src + JM_KWP00_HEADER_LENGTH, length);
    }
    return length;
}

JMKWP2000 *jm_kwp2000_new(void)
{
    JMKWP2000 *obj = (JMKWP2000*)g_malloc(sizeof(JMKWP2000));

    obj->user_data = NULL;

    return obj;
}

void jm_kwp2000_free(JMKWP2000 *self)
{
    g_return_if_fail(self != NULL);

    g_free(self);
}

size_t jm_kwp2000_pack(JMKWP2000 *self, const guint8 *src, size_t count, 
    guint8 *tar)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->pack != NULL, 0);

    return self->pack(self, src, count, tar);
}

size_t jm_kwp2000_unpack(JMKWP2000 *self, const guint8 *src, size_t count, 
    guint8 *tar)
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->unpack != NULL, 0);

    return self->unpack(self, src, count, tar);
}

gint32 jm_kwp2000_addr_init(JMKWP2000 *self, guint8 addr_code)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->addr_init != NULL, JM_ERROR_GENERIC);

    return self->addr_init(self, addr_code);
}

gint32 jm_kwp2000_fast_init(JMKWP2000 *self, const guint8 *data, 
    size_t count)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->fast_init != NULL, JM_ERROR_GENERIC);

    return self->fast_init(self, data, count);
}

gint32 jm_kwp2000_set_lines(JMKWP2000 *self, gint32 com_line, 
    gboolean l_line)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_lines != NULL, JM_ERROR_GENERIC);

    return self->set_lines(self, com_line, l_line);
}

gint32 jm_kwp2000_set_options(JMKWP2000 *self, JMKWPMode msg_mode, 
    JMKWPMode link_mode, gint32 baud)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_options != NULL, JM_ERROR_GENERIC);

    return self->set_options(self, msg_mode, link_mode, baud);
}

gint32 jm_kwp2000_set_address(JMKWP2000 *self, guint8 target, 
    guint8 source)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_address != NULL, JM_ERROR_GENERIC);

    return self->set_address(self, target, source);
}
