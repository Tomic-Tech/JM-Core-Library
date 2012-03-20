#include "jmv2commbox.h"
#include "jmserialport.h"
#include "jmv2boxlink.h"

static gint32 _jm_v2_commbox_open(JMCommbox *self)
{
    if (jm_v2_box_link_open())
        return JM_ERROR_SUCCESS;
    return JM_ERROR_COMMBOX_OPEN_FAIL;
}

JMCommbox* jm_v2_commbox_new(void)
{
    JMCommbox *obj = jm_commbox_new();

    obj->open = _jm_v2_commbox_open;
    obj->close = NULL;
    obj->configure = NULL;
    obj->set_connector = NULL;
    obj->free = NULL;
    obj->version = JM_COMMBOX_V2;
    obj->user_data = NULL;

    return obj;
}
