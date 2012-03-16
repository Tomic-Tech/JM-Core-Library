#ifndef __JM_V2_ISO15765_H__
#define __JM_V2_ISO15765_H__

#ifndef _MSC_VER
#pragma once
#endif

#include "jmcanbus.h"

G_BEGIN_DECLS

JMCanbus* jm_v2_iso15765_new(void);
gboolean jm_v2_iso15765_prepare(JMCanbus *self);
size_t jm_v2_iso15765_send_one_frame(JMCanbus *self, const guint8 *data, 
    size_t count);
size_t jm_v2_iso15765_send_frames(JMCanbus *self, const guint8 *data, 
    size_t count);
size_t jm_v2_iso15765_read_one_frame(JMCanbus *self, guint8 *data);
size_t jm_v2_iso15765_read_frames(JMCanbus *self, guint8 *data);
gint32 jm_v2_iso15765_set_keep_link(JMCanbus *self, const guint8 *data, 
    size_t count);

G_END_DECLS

#endif