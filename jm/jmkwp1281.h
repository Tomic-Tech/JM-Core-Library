#ifndef __JM_KWP1281_H__
#define __JM_KWP1281_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

#define JM_KWP1281_FRAME_END 0x03

typedef struct _JMKWP1281 JMKWP1281;

struct _JMKWP1281 {
    size_t (*pack)(JMKWP1281* self, const guint8 *src, size_t count, guint8 *tar);
    size_t (*unpack)(JMKWP1281* self, const guint8 *src, size_t count, guint8 *tar);
    gint32 (*addr_init)(JMKWP1281 *self, guint8 addr_code);
    gint32 (*set_lines)(JMKWP1281 *self, gint32 com_line, gboolean l_line);
    guint8 frame_counter;

    gpointer user_data;
};

JMKWP1281* jm_kwp1281_new(void);
void jm_kwp1281_free(JMKWP1281 *self);
size_t jm_kwp1281_pack(JMKWP1281 *self, const guint8 *src, size_t count, guint8 *tar);
size_t jm_kwp1281_unpack(JMKWP1281 *self, const guint8 *src, size_t count, guint8 *tar);
gint32 jm_kwp1281_addr_init(JMKWP1281 *self, guint8 addr_code);
gint32 jm_kwp1281_set_lines(JMKWP1281 *self, gint32 com_line, gboolean l_line);

G_END_DECLS

#endif
