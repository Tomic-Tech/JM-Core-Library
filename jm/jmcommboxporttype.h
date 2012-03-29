#ifndef __JM_COMMBOX_PORT_TYPE_H__
#define __JM_COMMBOX_PORT_TYPE_H__

#include <jm/jmtype.h>

JM_BEGIN_DECLS

typedef enum _JMCommboxPortType JMCommboxPortType;

enum _JMCommboxPortType
{
    JM_COMMBOX_PORT_SERIAL_PORT,
    JM_COMMBOX_PORT_UNKNOW
};

JM_END_DECLS

#endif