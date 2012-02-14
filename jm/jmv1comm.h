#ifndef __JM_V1_COMM_H__
#define __JM_V1_COMM_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmcomm.h"
#include "jmv1box.h"

G_BEGIN_DECLS

JMComm* jm_v1_comm_new(JMProtocolType type, JMV1Box *box);
void jm_v1_comm_free(JMComm *self);

G_END_DECLS

#endif