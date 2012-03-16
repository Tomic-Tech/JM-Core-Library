#include "jmv2boxbuffer.h"

static guint8 _jm_v2_box_buffer_send[JM_V2_BOX_HEAD_LENGTH + 
    JM_V2_BOX_MAX_DATA_LENGTH + JM_V2_BOX_CS_LENGTH];
static guint8 _jm_v2_box_buffer_recv[JM_V2_BOX_HEAD_LENGTH + 
    JM_V2_BOX_MAX_DATA_LENGTH + JM_V2_BOX_CS_LENGTH];

guint8* jm_v2_box_buffer_get(JMV2BoxBufferType type)
{
    switch(type)
    {
    case JM_V2_BOX_BUFFER_SEND:
        return _jm_v2_box_buffer_send;
    case JM_V2_BOX_BUFFER_RECV:
        return _jm_v2_box_buffer_recv;
    default:
        return NULL;
    }
}

guint8* jm_v2_box_buffer_head(JMV2BoxBufferType type)
{
    switch(type)
    {
    case JM_V2_BOX_BUFFER_SEND:
        return _jm_v2_box_buffer_send;
    case JM_V2_BOX_BUFFER_RECV:
        return _jm_v2_box_buffer_recv;
    default:
        return NULL;
    }
}

guint8* jm_v2_box_buffer_data(JMV2BoxBufferType type, size_t *buff_length)
{
    switch(type)
    {
    case JM_V2_BOX_BUFFER_SEND:
        *buff_length = (_jm_v2_box_buffer_send[2] << 8) + 
            _jm_v2_box_buffer_send[3];
        return _jm_v2_box_buffer_send + JM_V2_BOX_HEAD_LENGTH;
    case JM_V2_BOX_BUFFER_RECV:
        *buff_length = (_jm_v2_box_buffer_recv[2] << 8) +
            _jm_v2_box_buffer_recv[3];
        return _jm_v2_box_buffer_recv + JM_V2_BOX_HEAD_LENGTH;
    default:
        *buff_length = 0;
        return NULL;
    }
}

guint8* jm_v2_box_buffer_cs_pointer(JMV2BoxBufferType type)
{
    switch(type)
    {
    case JM_V2_BOX_BUFFER_SEND:
        return _jm_v2_box_buffer_send + JM_V2_BOX_HEAD_LENGTH +
            (_jm_v2_box_buffer_send[2] << 8) +
            _jm_v2_box_buffer_send[3];
    case JM_V2_BOX_BUFFER_RECV:
        return _jm_v2_box_buffer_recv + JM_V2_BOX_HEAD_LENGTH +
            (_jm_v2_box_buffer_recv[2] << 8) +
            _jm_v2_box_buffer_recv[3];
    default:
        return NULL;
    }
}

guint8 jm_v2_box_buffer_cs(JMV2BoxBufferType type)
{
    guint8 *temp = NULL;
    guint8 checksum = 0;
    size_t i;
    switch(type)
    {
    case JM_V2_BOX_BUFFER_SEND:
        temp = _jm_v2_box_buffer_send;
        break;
    case JM_V2_BOX_BUFFER_RECV:
        temp = _jm_v2_box_buffer_recv;
        break;
    default:
        return 0;
    }
    for (i = JM_V2_BOX_CMD_LENGTH; 
        i < jm_v2_box_buffer_length(type) - JM_V2_BOX_CS_LENGTH; ++i)
    {
        checksum ^= temp[i];
    }
    return checksum;
}

size_t jm_v2_box_buffer_length(JMV2BoxBufferType type)
{
    guint8 *temp = NULL;
    switch(type)
    {
    case JM_V2_BOX_BUFFER_SEND:
        temp = _jm_v2_box_buffer_send;
        break;
    case JM_V2_BOX_BUFFER_RECV:
        temp = _jm_v2_box_buffer_recv;
        break;
    default:
        return 0;
    }
    
    return JM_V2_BOX_HEAD_LENGTH + JM_V2_BOX_CS_LENGTH +  (temp[2] << 8) + 
        temp[3];
}

guint8* jm_v2_box_buffer_ecu(JMV2BoxBufferType type, size_t *buff_length)
{
    guint8 *temp = NULL;
    switch(type)
    {
    case JM_V2_BOX_BUFFER_SEND:
        temp = _jm_v2_box_buffer_send + JM_V2_BOX_HEAD_LENGTH + 
            JM_V2_BOX_CMD_LENGTH;
        *buff_length = (_jm_v2_box_buffer_send[2] << 8) +
            _jm_v2_box_buffer_send[3] - JM_V2_BOX_CMD_LENGTH;
        break;
    case JM_V2_BOX_BUFFER_RECV:
        temp = _jm_v2_box_buffer_recv + JM_V2_BOX_HEAD_LENGTH + 1;
        *buff_length = (_jm_v2_box_buffer_recv[2] << 8) + 
            _jm_v2_box_buffer_recv[3] - 1;
        break;
    default:
        break;
    }
    return temp;
}
