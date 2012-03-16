#ifndef __JM_V2_BOX_BUFFER_H__
#define __JM_V2_BOX_BUFFER_H__

#include <glib.h>

G_BEGIN_DECLS

#define JM_V2_BOX_HEAD_LENGTH 0x04
#define JM_V2_BOX_CMD_LENGTH 0x02
#define JM_V2_BOX_CS_LENGTH 0x01
#define JM_V2_BOX_MAX_DATA_LENGTH 0xFFFF

typedef enum _JMV2BoxBufferType
{
    JM_V2_BOX_BUFFER_SEND,
    JM_V2_BOX_BUFFER_RECV
} JMV2BoxBufferType;

guint8* jm_v2_box_buffer_get(JMV2BoxBufferType type);
guint8* jm_v2_box_buffer_head(JMV2BoxBufferType type);
guint8* jm_v2_box_buffer_data(JMV2BoxBufferType type, size_t *buff_length);
guint8* jm_v2_box_buffer_ecu(JMV2BoxBufferType type, size_t *buff_length);
guint8* jm_v2_box_buffer_cs_pointer(JMV2BoxBufferType type);
guint8 jm_v2_box_buffer_cs(JMV2BoxBufferType type);
size_t jm_v2_box_buffer_length(JMV2BoxBufferType type);


G_END_DECLS

#endif