#include "jmv2boxoption.h"
#include <stdio.h>
#include "jmv2boxlink.h"
#include "jmv2boxbuffer.h"
#include "jmtimer.h"
#include "jmcommboxport.h"

static guint8 _jm_v2_box_version[JM_V2_BOX_VERSION_LENGTH];

gboolean jm_v2_box_is_config_success(void)
{
    size_t length;
    guint8 *temp;
    if (jm_v2_box_link_read(JM_TIMER_TO_SEC(1)) == 0)
        return FALSE;
    jm_commbox_port_discard_in_buffer();

    temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_RECV, &length);

    if (temp[0] != JM_V2_BOX_STATUS)
    {
        return FALSE;
    }

    if (temp[1] != JM_V2_BOX_CONFIG_SUCCESS)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean jm_v2_box_read_version(void)
{
    size_t length;
    guint8 *temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_SEND, &length);

    temp[0] = JM_V2_BOX_SET_COMMBOX;
    temp[1] = JM_V2_BOX_GET_MCU_VER;

    length = 2;
    jm_v2_box_link_send(length);

    if (jm_v2_box_link_read(JM_TIMER_TO_SEC(1)) == 0 )
    {
        return FALSE;
    }

    temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_RECV, &length);
    _jm_v2_box_version[0] = 'B';
    sprintf(_jm_v2_box_version + 1, "%02d%02d%02d%02d", temp[5], temp[6], 
        temp[7], temp[8]);
    _jm_v2_box_version[9] = 0x00;
    return TRUE;
}

guint8* jm_v2_box_get_version(void)
{
    return _jm_v2_box_version;
}

gboolean jm_v2_box_set_can_mode(gint32 h_line, gint32 l_line,
    JMCanbusBaud baud)
{
    size_t length = 0;
    guint8 *temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_RECV, &length);

    temp[0] = JM_V2_BOX_SET_COMMBOX;
    temp[1] = JM_V2_BOX_SET_ECU_COM;
    temp[2] = JM_V2_BOX_SET_CAN_MODE;
    temp[3] = (guint8)(h_line);
    temp[4] = (guint8)(l_line);
    switch(baud)
    {
    case JM_CANBUS_B1000K:
        temp[5] = 0x01;
        break;
    case JM_CANBUS_B800K:
        temp[5] = 0x02;
        break;
    case JM_CANBUS_B500K:
        temp[5] = 0x03;
        break;
    case JM_CANBUS_B250K:
        temp[5] = 0x04;
        break;
    case JM_CANBUS_B125K:
        temp[5] = 0x05;
        break;
    case JM_CANBUS_B100K:
        temp[5] = 0x06;
        break;
    default:
        temp[5] = 0x03;
    }

    length = 6;
    jm_v2_box_link_send(length);
    return jm_v2_box_is_config_success();
}

gboolean jm_v2_box_set_can_afmr(JMCanbusIDMode id_mode, 
    JMCanbusFilterMask mask, size_t id_count,
    const guint32 *id_buffer)
{
    size_t length = 0;
    guint8 *temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_SEND, &length);
    temp[0] = JM_V2_BOX_SET_COMMBOX;
    temp[1] = JM_V2_BOX_SET_CANID_AFRM;
    temp[2] = (guint8)(id_mode | mask);
    temp[3] = (guint8)(id_count);
    length = 4;
    if (id_mode == JM_CANBUS_ID_MODE_EXT)
    {
        /* T.B.C */
    }
    else
    {
        size_t i;
        length += id_count * 2;
        for (i = 0; i < id_count; ++i)
        {
            temp[4 + i * 2] = (guint8)((id_buffer[i] & 0x0000FF00));
            temp[5 + i * 2] = (guint8)((id_buffer[i] & 0x000000FF));
        }
    }
    jm_v2_box_link_send(length);
    return jm_v2_box_is_config_success();
}

gboolean jm_v2_box_set_kwp_mode(gint32 com_line, guint32 baud, 
    gboolean l_line)
{
    size_t length = 0;
    guint8 *temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_SEND, &length);
    
    temp[0] = JM_V2_BOX_SET_COMMBOX;
    temp[1] = JM_V2_BOX_SET_ECU_COM;
    temp[2] = JM_V2_BOX_SET_KWP_MODE;
    temp[3] = (guint8)com_line;
    temp[4] = (guint8)com_line;
    temp[5] = (guint8)(((baud) & 0x00FF0000) >> 16);
    temp[6] = (guint8)(((baud) & 0x0000FF00) >> 8);
    temp[7] = (guint8)((baud) & 0x000000FF);

    if (l_line)
    {
        temp[8] = 0xFF;
    }
    else
    {
        temp[8] = 0x00;
    }
    length = 9;
    jm_v2_box_link_send(length);
    return jm_v2_box_is_config_success();
}

gboolean jM_v2_box_set_kep_signal_time(guint8 low, guint8 high)
{
    size_t length = 0;
    guint8* temp = jm_v2_box_buffer_data(JM_V2_BOX_BUFFER_SEND, &length);

    temp[0] = JM_V2_BOX_SET_COMMBOX;
    temp[1] = JM_V2_BOX_SET_QUICK_KWP;
    temp[2] = (guint8)(low * 2);
    temp[3] = (guint8)(high * 2);
    length = 4;
    jm_v2_box_link_send(length);
    return jm_v2_box_is_config_success();
}