#ifndef __JM_CANBUS_FILTER_MASK_H__
#define __JM_CANBUS_FILTER_MASK_H__

#ifdef _MSC_VER
#pragma once
#endif

JM_BEGIN_DECLS

typedef enum _JMCanbusFilterMask JMCanbusFilterMask;

enum _JMCanbusFilterMask
{
    JM_CANBUS_FILTER_MASK_ENABLE = 0x0F,
    JM_CANBUS_FILTER_MASK_DISABLE = 0x00
};

JM_END_DECLS

#endif