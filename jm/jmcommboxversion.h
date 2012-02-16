#ifndef __JM_COMMBOX_VERSION_H__
#define __JM_COMMBOX_VERSION_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMCommboxVersion JMCommboxVersion;

enum _JMCommboxVersion {
	JM_COMMBOX_V1 = 0,
	JM_COMMBOX_ELM327 = 1,
	JM_COMMBOX_TL718 = 2,
    JM_COMMBOX_UNKNOW
};

G_END_DECLS

#endif