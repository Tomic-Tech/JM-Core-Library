#ifndef __JM_PROTOCOL_TYPE_H__
#define __JM_PROTOCOL_TYPE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmtype.h>

JM_BEGIN_DECLS

typedef enum _JMProtocolType JMProtocolType;

enum _JMProtocolType
{
	JM_PRC_J1850VPW,
	JM_PRC_J1850PWM,
	JM_PRC_ISO9141_2,
	JM_PRC_ISO14230,
	JM_PRC_ISO15765,
	JM_PRC_ALDL160,
	JM_PRC_ALDL8192,
	JM_PRC_KWP1281,
	JM_PRC_MIKUNI,
    JM_PRC_UNKNOW
};

JM_END_DECLS

#endif