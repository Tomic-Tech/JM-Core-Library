#ifndef __JM_V1_COMMBOX_H__
#define __JM_V1_COMMBOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmcommbox.h"

G_BEGIN_DECLS

JMCommbox* jm_v1_commbox_new(void);
void jm_v1_commbox_free(JMCommbox *self);

G_END_DECLS

#endif