#ifndef __JM_PROTOCOL_TYPE_H__
#define __JM_PROTOCOL_TYPE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMProtocolType JMProtocolType;

enum _JMProtocolType {
    JM_PRC_UNKNOW = 0,
	JM_PRC_J1850VPW = 1,
	JM_PRC_J1850PWM = 2,
	JM_PRC_ISO9141_2 = 3,
	JM_PRC_ISO14230 = 4,
	JM_PRC_ISO15765 = 5,
	JM_PRC_ALDL160 = 6,
	JM_PRC_ALDL8192 = 7,
	JM_PRC_KWP1281 = 8,
	JM_PRC_MIKUNI = 9
};

G_END_DECLS

#endif