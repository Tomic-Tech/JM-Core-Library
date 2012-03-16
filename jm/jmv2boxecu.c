#include "jmv2boxecu.h"
#include "jmv2boxbuffer.h"
#include "jmv2boxlink.h"
#include "jmv2boxoption.h"
#include "jmutils.h"

size_t jm_v2_box_ecu_send(guint8 type, size_t count)
{
    size_t length = 0;
    guint8 *temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_SEND, &length);

    temp[0] = JM_V2_BOX_SEND_TO_ECU;
    temp[1] = type;

    length = JM_V2_BOX_CMD_LENGTH + count;
    return jm_v2_box_link_send(length);
}

size_t jm_v2_box_ecu_read(gint64 microseconds)
{
    guint8 *temp;
    size_t length = jm_v2_box_link_read(microseconds);

    if (length == 0)
        return length;

    temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_RECV, &length);

    if (temp[0] != JM_V2_BOX_ECU_DATA)
    {
        if (temp[0] == JM_V2_BOX_STATUS)
        {
            return 0;
        }
        else
        {
            return 0;
        }
    }
    return length;
}

size_t jm_v2_box_ecu_kwp_req_next_msg(gint64 microseconds)
{
    size_t length;
    guint8 *temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_SEND, &length);

    temp[0] = JM_V2_BOX_SEND_TO_ECU;
    temp[1] = JM_V2_BOX_K_REQ_NEXT;
    temp[2] = JM_HIGH_BYTE(microseconds * 2);
    temp[3] = JM_LOW_BYTE(microseconds * 2);

    length = 4;
    return jm_v2_box_link_send(length);
}
