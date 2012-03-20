#ifndef __JM_COMMBOX_PORT_TYPE_H__
#define __JM_COMMBOX_PORT_TYPE_H__

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMCommboxPortType JMCommboxPortType;

enum _JMCommboxPortType
{
    JM_COMMBOX_PORT_SERIAL_PORT,
    JM_COMMBOX_PORT_UNKNOW
};

G_END_DECLS

#endif