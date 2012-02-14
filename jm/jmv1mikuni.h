#ifndef __JM_V1_MIKUNI_H__
#define __JM_V1_MIKUNI_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmmikuni.h"
#include "jmv1box.h"

G_BEGIN_DECLS

JMMikuni* jm_v1_mikuni_new(JMV1Box *box);
void jm_v1_mikuni_free(JMMikuni *self);
gint32 jm_v1_mikuni_set_keep_link(JMMikuni *self, const guint8 *data, size_t count);
size_t jm_v1_mikuni_read_frames(JMMikuni *self, guint8 *data);
size_t jm_v1_mikuni_read_one_frame(JMMikuni *self, guint8 *data);
size_t jm_v1_mikuni_send_frames(JMMikuni *self, const guint8 *data, size_t count);
size_t jm_v1_mikuni_send_one_frame(JMMikuni *self, const guint8 *data, size_t count);

G_END_DECLS

#endif