#ifndef __JM_V1_ISO15765_H__
#define __JM_V1_ISO15765_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmcanbus.h"
#include "jmv1box.h"

G_BEGIN_DECLS

JMCanbus* jm_v1_iso15765_new(JMV1Box *box);
gboolean jm_v1_iso15765_prepare(JMCanbus *self);
size_t jm_v1_iso15765_send_one_frame(JMCanbus *self, const guint8 *data, 
    size_t count);
size_t jm_v1_iso15765_send_frames(JMCanbus *self, const guint8 *data, 
    size_t count);
size_t jm_v1_iso15765_read_one_frame(JMCanbus *self, guint8 *data);
size_t jm_v1_iso15765_read_frames(JMCanbus *self, guint8 *data);
gint32 jm_v1_iso15765_set_keep_link(JMCanbus *self, const guint8 *data, 
    size_t count);

G_END_DECLS

#endif