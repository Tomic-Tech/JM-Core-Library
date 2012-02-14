#ifndef __JM_V1_DEFAULT_H__
#define __JM_V1_DEFAULT_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

#define JM_V1_DEFAULT_SEND_ONE_FRAME(shared, box, base, data, count, need_recv, prc) \
G_STMT_START { \
    guint8 send_buff[256]; \
    size_t length = (base)->pack((base), (data), (count), send_buff); \
    (shared)->buff_id = 0; \
    if (!(box)->new_batch((box), (shared)->buff_id)) { \
        return 0;\
    } \
    \
    if (length <= 0) \
        return 0; \
    \
    if ((need_recv)) { \
        if (!(box)->send_out_data((box), send_buff, length) \
            || !(box)->run_receive((box), JM_V1_BOX_C((box), RECEIVE)) \
            || !(box)->end_batch((box)) \
            || !(box)->run_batch((box), &((shared)->buff_id), 1, FALSE)) { \
                return 0; \
        } \
    } else { \
        if (!(box)->send_out_data((box), send_buff, length) \
            || !(box)->end_batch((box)) \
            || !(box)->run_batch((box), &((shared)->buff_id), 1, FALSE)) { \
                return 0; \
        } \
    } \
    _jm_v1_##prc##_finish_execute(base, !(need_recv)); \
    return length; \
} G_STMT_END

#define JM_V1_DEFAULT_SET_KEEP_LINK(box, data, count) \
G_STMT_START { \
    if (!box->new_batch(box, JM_V1_BOX_C(box, LINKBLOCK))) { \
        return JM_ERROR_GENERIC; \
    } \
    if (!box->send_out_data(box, data, count) \
        || !box->end_batch(box)) {\
        return JM_ERROR_GENERIC; \
    } \
    return JM_ERROR_SUCCESS; \
} G_STMT_END

G_END_DECLS

#endif