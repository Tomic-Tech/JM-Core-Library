#ifndef __DIAG_CONNECTOR_HPP__
#define __DIAG_CONNECTOR_HPP__

#ifdef _MSC_VER
#pragma once
#endif

namespace JM
{
namespace Diag
{
enum Connector
{
    CN_OBDII_16,
    CN_UNIVERSAL_3,
    CN_BENZ_38,
    CN_BMW_20,
    CN_AUDI_4,
    CN_FIAT_3,
    CN_CITROEN_2,
    CN_CHRYSLER_6,
    CN_TOYOTA_17R,
    CN_TOYOTA_17F,
    CN_HONDA_3,
    CN_MITSUBISHI,
    CN_HYUNDAI,
    CN_NISSAN,
    CN_SUZUKI_3,
    CN_DAIHATSU_4,
    CN_ISUZU_3,
    CN_CANBUS_16,
    CN_GM_12,
    CN_KIA_20,
    CN_NISSAN_14,
    CN_UNKNOW
};
}
}

#endif