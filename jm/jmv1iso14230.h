#ifndef __JM_V1_ISO14230_H__
#define __JM_V1_ISO14230_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmkwp2000.h"
#include "jmv1box.h"

G_BEGIN_DECLS

JMKWP2000* jm_v1_iso14230_new(JMV1Box *box);
void jm_v1_iso14230_free(JMKWP2000 *self);
size_t jm_v1_iso14230_send_one_frame(JMKWP2000 *self, const guint8 *data, size_t count);
size_t jm_v1_iso14230_send_frames(JMKWP2000 *self, const guint8 *data, size_t count);
size_t jm_v1_iso14230_read_one_frame(JMKWP2000 *self, guint8 *data);
size_t jm_v1_iso14230_read_frames(JMKWP2000 *self, guint8 *data);
gint32 jm_v1_iso14230_set_keep_link(JMKWP2000 *self, const guint8 *data, size_t count);

G_END_DECLS

#endif