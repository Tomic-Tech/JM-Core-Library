#ifndef __JM_CANBUS_FRAME_TYPE_H__
#define __JM_CANBUS_FRAME_TYPE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMCanbusFrameType JMCanbusFrameType;

enum _JMCanbusFrameType
{
    JM_CANBUS_FRAME_TYPE_DATA = 0x00,
    JM_CANBUS_FRAME_TYPE_REMOTE = 0x40
};

G_END_DECLS

#endif