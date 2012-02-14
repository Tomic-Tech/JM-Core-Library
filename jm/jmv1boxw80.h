#ifndef __JM_V1_BOX_W80_H__
#define __JM_V1_BOX_W80_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmv1box.h"

G_BEGIN_DECLS

typedef struct _JMV1BoxW80 JMV1BoxW80;

JMV1Box* jm_v1_box_w80_new(JMV1Shared *shared);
void jm_v1_box_w80_free(JMV1Box *self);

G_END_DECLS

#endif