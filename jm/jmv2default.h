#ifndef __JM_V2_DEFAULT_H__
#define __JM_V2_DEFAULT_H__

#include "jmv2boxecu.h"
#include "jmv2boxbuffer.h"

#define jm_v2_default_send_one_frame(self, msg_type, cmd, length) \
{ \
    size_t temp_length = 0; \
    guint8 *temp = jm_v2_box_buffer_ecu(JM_V2_BOX_BUFFER_SEND, &temp_length); \
    temp_length = self->pack(self, cmd, length, temp); \
    if (length == 0) \
    { \
        return 0; \
    } \
    return jm_v2_box_ecu_send(msg_type, temp_length); \
}

#define jm_v2_default_read_one_frame(self, data_buffer, microseconds) \
{ \
    size_t length; \
    guint8 *source; \
    if (data_buffer == NULL) \
    { \
        return 0; \
    } \
    length = jm_v2_box_ecu_read(microseconds); \
    if (length == 0) \
    { \
        return length; \
    }\
    source = jm_v2_box_buffer_ecu(JM_V2_BOX_BUFFER_RECV, &length); \
    length = self->unpack(self, source, length, data_buffer); \
    if (length == 0) \
    { \
        return length; \
    } \
    return length; \
}    


#endif