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
            guint32 last_error; /* 提供错误查询 */
            gboolean is_db_20;
            gboolean is_do_now;
            guint32 req_byte_to_byte;
            guint32 req_wait_time;
            guint32 res_wait_time;
            guint32 res_byte_to_byte;
            guint32 next_address;
            guint8 buff_id;
        };
    }
}

#endif
