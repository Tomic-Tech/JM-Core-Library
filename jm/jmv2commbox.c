#include "jmv2commbox.h"
#include "jmserialport.h"
#include "jmv2boxlink.h"

static gint32 _jm_v2_commbox_open(JMCommbox *self)
{
    if (jm_v2_box_link_open())
        return JM_ERROR_SUCCESS;
    return JM_ERROR_COMMBOX_OPEN_FAIL;
}

static gint32 _jm_v2_commbox_serial_port_baud(JMCommbox *self)
{
    return 57600;
}

static guint8 _jm_v2_commbox_serial_port_databits(JMCommbox *self)
{
    return 8;
}

static gint32 _jm_v2_commbox_serial_port_parity(JMCommbox *self)
{
    return JM_SP_PAR_NONE;
}

static gint32 _jm_v2_commbox_serial_port_stopbits(JMCommbox *self)
{
    return JM_SP_SB_ONE;
}

static gint32 _jm_v2_commbox_serial_port_flow_control(JMCommbox *self)
{
    return JM_SP_FC_NONE;
}

static gboolean _jm_v2_commbox_serial_port_change_config(JMCommbox *self)
{
    if (jm_v2_box_link_is_connected())
        return FALSE;
    return TRUE;
}

static gboolean 
    _jm_v2_commbox_check_serial_port_change_config(JMCommbox *self)
{
    return jm_v2_box_link_is_connected();
}

JMCommbox* jm_v2_commbox_new(void)
{
    JMCommbox *obj = jm_commbox_new();

    obj->open = _jm_v2_commbox_open;
    obj->close = NULL;
    obj->configure = NULL;
    obj->set_connector = NULL;
    obj->serial_port_baud = _jm_v2_commbox_serial_port_baud;
    obj->serial_port_databits = _jm_v2_commbox_serial_port_databits;
    obj->serial_port_parity = _jm_v2_commbox_serial_port_parity;
    obj->serial_port_stopbits = _jm_v2_commbox_serial_port_stopbits;
    obj->serial_port_flow_control = _jm_v2_commbox_serial_port_flow_control;
    obj->serial_port_change_config = 
        _jm_v2_commbox_serial_port_change_config;
    obj->check_serial_port_change_config = 
        _jm_v2_commbox_check_serial_port_change_config;
    obj->free = NULL;
    obj->version = JM_COMMBOX_V2;
    obj->user_data = NULL;

    return obj;
}
