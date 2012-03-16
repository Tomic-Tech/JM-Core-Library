#include "jmv2boxlink.h"
#include "jmcommboxport.h"
#include "jmv2boxbuffer.h"
#include "jmserialport.h"

gboolean jm_v2_box_link_open(void)
{
    return TRUE;
}

void jm_v2_box_link_close(void)
{
}

gboolean jm_v2_box_link_is_connected(void)
{
    return TRUE;
}

size_t _jm_v2_box_link_read(guint8 *buffer, size_t length, gint64 microseconds)
{
    jm_commbox_port_set_read_timeout(microseconds);
    return jm_commbox_port_read(buffer, length);
}

size_t jm_v2_box_link_read(gint64 microseconds)
{
    size_t data_length;
    guint8 *data_buffer;
    guint8 *cs_buffer;
    guint8 *header_buffer = jm_v2_box_buffer_get(JM_V2_BOX_BUFFER_RECV);
    if (_jm_v2_box_link_read(header_buffer, JM_V2_BOX_HEAD_LENGTH, 
        microseconds) != JM_V2_BOX_HEAD_LENGTH)
    {
        return 0;
    }

    data_buffer = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_RECV, &data_length);

    if (_jm_v2_box_link_read(data_buffer, data_length, microseconds) != 
        data_length)
    {
        return 0;
    }

    cs_buffer = jm_v2_box_buffer_cs_pointer(JM_V2_BOX_BUFFER_RECV);
    if (_jm_v2_box_link_read(cs_buffer, JM_V2_BOX_CS_LENGTH, microseconds) !=
        JM_V2_BOX_CS_LENGTH)
    {
        return 0;
    }

    if (cs_buffer[0] != jm_v2_box_buffer_cs(JM_V2_BOX_BUFFER_RECV))
    {
        return 0;
    }
    return jm_v2_box_buffer_length(JM_V2_BOX_BUFFER_RECV);

}

size_t jm_v2_box_link_send(size_t length)
{
    guint8 *temp = jm_v2_box_buffer_get(JM_V2_BOX_BUFFER_SEND);
    size_t buff_length;

    temp[0] = JM_V2_BOX_HEADER_1;
    temp[1] = JM_V2_BOX_HEADER_2;
    temp[2] = (guint8)(length >> 8);
    temp[3] = (guint8)length;
    
    buff_length = jm_v2_box_buffer_length(JM_V2_BOX_BUFFER_SEND);

    temp[buff_length - 1] = jm_v2_box_buffer_cs(JM_V2_BOX_BUFFER_SEND);
    return jm_commbox_port_write(temp, buff_length);
}

void jm_v2_box_link_reset(void)
{
    JMSerialPort *port = jm_commbox_port_get_pointer();

    if (jm_commbox_port_get_type() == JM_COMMBOX_PORT_SERIAL_PORT)
    {
        jm_serial_port_set_rts(port, TRUE);
        g_usleep(1000);
        jm_serial_port_set_rts(port, FALSE);
        g_usleep(1000);
        jm_serial_port_discard_in_buffer(port);
    }
}

