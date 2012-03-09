#ifndef __JM_MIKUNI_H__
#define __JM_MIKUNI_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _JMMikuni JMMikuni;

#define JM_MIKUNI_HEAD_FORMAT 0x48

struct _JMMikuni
{
    size_t (*pack)(JMMikuni *self, const guint8 *src, size_t count, guint8 *tar);
    size_t (*unpack)(JMMikuni *self, const guint8 *src, size_t count, guint8 *tar);
    gint32 (*init)(JMMikuni *self);
    void (*free)(gpointer user_data);

    gpointer user_data;
};

GLIB_VAR JMMikuni* jm_mikuni_new(void);
GLIB_VAR void jm_mikuni_free(JMMikuni *self);
GLIB_VAR size_t jm_mikuni_pack(JMMikuni *self, const guint8 *src, size_t count, guint8 *tar);
GLIB_VAR size_t jm_mikuni_unpack(JMMikuni *self, const guint8 *src, size_t count, guint8 *tar);
GLIB_VAR gint32 jm_mikuni_init(JMMikuni *self);

G_END_DECLS

#endif