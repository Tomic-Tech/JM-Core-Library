#ifndef __JM_CANBUS_ID_MODE_H__
#define __JM_CANBUS_ID_MODE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMCanbusIDMode JMCanbusIDMode;

enum _JMCanbusIDMode
{
    JM_CANBUS_ID_MODE_STD = 0x00,
    JM_CANBUS_ID_MODE_EXT = 0x80
};

G_END_DECLS

#endif