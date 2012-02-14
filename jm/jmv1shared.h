#ifndef __JM_V1_SHARED_H__
#define __JM_V1_SHARED_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmconnector.h"

G_BEGIN_DECLS

typedef struct _JMV1Shared JMV1Shared;

struct _JMV1Shared {
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

G_END_DECLS

#endif
