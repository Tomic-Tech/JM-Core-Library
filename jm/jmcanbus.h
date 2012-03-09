#ifndef __JM_CANBUS_H__
#define __JM_CANBUS_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmcanbusbaud.h>
#include <jm/jmcanbusfiltermask.h>
#include <jm/jmcanbusframetype.h>
#include <jm/jmcanbusidmode.h>

G_BEGIN_DECLS

typedef struct _JMCanbus JMCanbus;

struct _JMCanbus
{
    size_t (*pack)(JMCanbus *self,
        const guint8 *src,
        size_t count,
        guint8 *tar);
    size_t (*unpack)(JMCanbus *self,
        const guint8 *src,
        size_t count,
        guint8 *tar);
    gint32 (*set_lines)(JMCanbus *self,
        gint32 high,
        gint32 low);
    gint32 (*set_filter)(JMCanbus *self,
        const gint32 *id_array,
        size_t count);
    gint32 (*set_options)(JMCanbus *self,
        gint32 id,
        JMCanbusBaud baud,
        JMCanbusIDMode id_mode,
        JMCanbusFilterMask mask,
        JMCanbusFrameType frame);
    void (*free)(gpointer user_data);

    gint32 target_id;
    JMCanbusBaud baud;
    JMCanbusIDMode id_mode;
    JMCanbusFilterMask filter_mask;
    JMCanbusFrameType frame_type;
    gint32 high;
    gint32 low;
    GPtrArray *id_array;
    guint8 flow_control[8];

    gpointer user_data;
};

GLIB_VAR gint32 _jm_canbus_set_options(JMCanbus *self, 
    gint32 id, 
    JMCanbusBaud baud, 
    JMCanbusIDMode id_mode, 
    JMCanbusFilterMask mask, 
    JMCanbusFrameType frame);
GLIB_VAR JMCanbus* jm_canbus_new(void);
GLIB_VAR void jm_canbus_free(JMCanbus *self);
GLIB_VAR size_t jm_canbus_pack(JMCanbus *self,
    const guint8 *src,
    size_t count,
    guint8 *tar);
GLIB_VAR size_t jm_canbus_unpack(JMCanbus *self,
    const guint8 *src,
    size_t count,
    guint8 *tar);
GLIB_VAR gint32 jm_canbus_set_lines(JMCanbus *self,
    gint32 high,
    gint32 low);
GLIB_VAR gint32 jm_canbus_set_filter(JMCanbus *self,
    const gint32 *id_array,
    size_t count);
GLIB_VAR gint32 jm_canbus_set_options(JMCanbus *self,
    gint32 id,
    JMCanbusBaud baud,
    JMCanbusIDMode id_mode,
    JMCanbusFilterMask mask,
    JMCanbusFrameType frame);

G_END_DECLS

#endif