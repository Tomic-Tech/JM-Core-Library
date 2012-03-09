#ifndef __JM_V1_KWP1281_H__
#define __JM_V1_KWP1281_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmkwp1281.h"
#include "jmv1box.h"

G_BEGIN_DECLS

JMKWP1281* jm_v1_kwp1281_new(JMV1Box *box);
size_t jm_v1_kwp1281_send_one_frame(JMKWP1281 *self, const guint8 *data, size_t count);
size_t jm_v1_kwp1281_send_frames(JMKWP1281 *self, const guint8 *data, size_t count);
size_t jm_v1_kwp1281_read_one_frame(JMKWP1281 *self, guint8 *data);
size_t jm_v1_kwp1281_read_frames(JMKWP1281 *self, guint8 *data);
size_t jm_v1_kwp1281_send_and_recv(JMKWP1281 *self, const guint8 *send, size_t count, guint8 *recv);
gint32 jm_v1_kwp1281_set_keep_link(JMKWP1281 *self, const guint8 *data, size_t count);

G_END_DECLS

#endif