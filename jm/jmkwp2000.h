#ifndef __JM_KWP2000_H__
#define __JM_KWP2000_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmkwpmode.h>

G_BEGIN_DECLS

#define JM_KWP8X_HEADER_LENGTH 3
#define JM_KWPCX_HEADER_LENGTH 3
#define JM_KWP80_HEADER_LENGTH 4
#define JM_KWPXX_HEADER_LENGTH 1
#define JM_KWP00_HEADER_LENGTH 2
#define JM_KWP_CHECKSUM_LENGTH 1
#define JM_KWP_MAX_DATA_LENGTH 128

typedef struct _JMKWP2000 JMKWP2000;

struct _JMKWP2000
{

    size_t (*pack)(JMKWP2000 *self, const guint8 *src, size_t count, guint8 *tar);
    size_t (*unpack)(JMKWP2000 *self, const guint8 *src, size_t count, guint8 *tar);
    gint32 (*addr_init)(JMKWP2000 *self, guint8 addr_code);
    gint32 (*fast_init)(JMKWP2000 *self, const guint8 *data, size_t count);
    gint32 (*set_lines)(JMKWP2000 *self, gint32 com_line, gboolean l_line);
    gint32 (*set_options)(JMKWP2000 *self, JMKWPMode msg_mode, JMKWPMode link_mode, gint32 baud);
    gint32 (*set_address)(JMKWP2000 *self, guint8 target, guint8 source);

    JMKWPMode mode;
    JMKWPMode link_mode;
    JMKWPMode msg_mode;
    gint32 baud;
    guint32 target_address;
    guint32 source_address;

    gpointer user_data;
};

GLIB_VAR JMKWP2000* jm_kwp2000_new(void);
GLIB_VAR void jm_kwp2000_free(JMKWP2000 *self);
GLIB_VAR size_t jm_kwp2000_pack(JMKWP2000 *self, const guint8 *src, size_t count, guint8 *tar);
GLIB_VAR size_t jm_kwp2000_unpack(JMKWP2000 *self, const guint8 *src, size_t count, guint8 *tar);
GLIB_VAR gint32 jm_kwp2000_addr_init(JMKWP2000 *self, guint8 addr_code);
GLIB_VAR gint32 jm_kwp2000_fast_init(JMKWP2000 *self, const guint8 *data, size_t count);
GLIB_VAR gint32 jm_kwp2000_set_lines(JMKWP2000 *self, gint32 com_line, gboolean l_line);
GLIB_VAR gint32 jm_kwp2000_set_options(JMKWP2000 *self, JMKWPMode msg_mode, JMKWPMode link_mode, gint32 baud);
GLIB_VAR gint32 jm_kwp2000_set_address(JMKWP2000 *self, guint32 target, guint32 source);

G_END_DECLS

#endif