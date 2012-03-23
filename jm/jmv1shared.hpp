#ifndef __JM_V1_SHARED_HPP__
#define __JM_V1_SHARED_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmconnector.h"

namespace JM
{
    namespace V1
    {
        class Shared
        {
        public:
            JMConnector connector;
            guint32 lastError; /* 提供错误查询 */
            gboolean isDB20;
            gboolean isDoNow;
            guint32 reqByteToByte;
            guint32 reqWaitTime;
            guint32 resWaitTime;
            guint32 resByteToByte;
            guint32 nextAddress;
            guint8 buffID;
        };
    }
}

#endif
