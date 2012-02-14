#ifndef __JM_CONNECTOR_H__
#define __JM_CONNECTOR_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMConnector JMConnector;

enum _JMConnector {
    JM_CN_UNKNOW = 0,
	JM_CN_OBDII_16,
	JM_CN_UNIVERSAL_3,
	JM_CN_BENZ_38,
	JM_CN_BMW_20,
	JM_CN_AUDI_4,
	JM_CN_FIAT_3,
	JM_CN_CITROEN_2,
	JM_CN_CHRYSLER_6,
	JM_CN_TOYOTA_17R,
	JM_CN_TOYOTA_17F,
	JM_CN_HONDA_3,
	JM_CN_MITSUBISHI,
	JM_CN_HYUNDAI,
	JM_CN_NISSAN,
	JM_CN_SUZUKI_3,
	JM_CN_DAIHATSU_4,
	JM_CN_ISUZU_3,
	JM_CN_CANBUS_16,
	JM_CN_GM_12,
	JM_CN_KIA_20,
	JM_CN_NISSAN_14
};

G_END_DECLS

#endif