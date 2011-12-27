/* 
 * File:   jm_device_connector_type.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:09
 */

#ifndef JM_CONNECTOR_TYPE_HPP
#define	JM_CONNECTOR_TYPE_HPP

#ifdef _MSC_VER
#pragma once
#endif

namespace jm {

enum connector_type {
    OBDII_16 = 0,
    UNIVERSAL_3,
    BENZ_38,
    BMW_20,
    AUDI_4,
    FIAT_3,
    CITROEN_2,
    CHRYSLER_6,
    TOYOTA_17R,
    TOYOTA_17F,
    HONDA_3,
    MITSUBISHI,
    HYUNDAI,
    NISSAN,
    SUZUKI_3,
    DAIHATSU_4,
    ISUZU_3,
    CANBUS_16,
    GM_12,
    KIA_20,
    NISSAN_14
};

}


#endif	/* JM_DEVICE_CONNECTOR_TYPE_HPP */

