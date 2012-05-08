#ifndef __JM_DIAG_PROTOCOL_TYPE_H__
#define __JM_DIAG_PROTOCOL_TYPE_H__

#ifdef _MSC_VER
#pragma once
#endif

namespace JM
{
namespace Diag
{

enum ProtocolType
{
    PRC_J1850VPW,
    PRC_J1850PWM,
    PRC_ISO9141_2,
    PRC_ISO14230,
    PRC_ISO15765,
    PRC_ALDL160,
    PRC_ALDL8192,
    PRC_KWP1281,
    PRC_MIKUNI,
    PRC_BESTURN_KWP,
    PRC_UNKNOW
};
}
}

#endif