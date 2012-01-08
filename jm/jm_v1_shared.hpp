/* 
 * File:   jm_v1_shared.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午5:01
 */

#ifndef JM_V1_SHARED_HPP
#define	JM_V1_SHARED_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jm_types.hpp>
#include <boost/smart_ptr.hpp>
#include <jm/jm_connector_type.hpp>

namespace jm {
namespace v1 {
class v1_shared {
public:
//    static const uint8 REC_FR = 0xE0;
//    static const uint8 REC_LEN_1 = 0xE1;
//    static const uint8 REC_LEN_2 = 0xE2;
//    static const uint8 REC_LEN_3 = 0xE3;
//    static const uint8 REC_LEN_4 = 0xE4;
//    static const uint8 REC_LEN_5 = 0xE5;
//    static const uint8 REC_LEN_6 = 0xE6;
//    static const uint8 REC_LEN_7 = 0xE7;
//    static const uint8 REC_LEN_8 = 0xE8;
//    static const uint8 REC_LEN_9 = 0xE9;
//    static const uint8 REC_LEN_10 = 0xEA;
//    static const uint8 REC_LEN_11 = 0xEB;
//    static const uint8 REC_LEN_12 = 0xEC;
//    static const uint8 REC_LEN_13 = 0xED;
//    static const uint8 REC_LEN_14 = 0xEE;
//    static const uint8 REC_LEN_15 = 0xEF;
//    static const uint8 RECEIVE = 0xF0;
//    static const uint32 LINKBLOCK = 0x40;
public:
    connector_type connector;
    uint8 buff_id;
    uint32 last_error; //提供错误查询
    bool is_db_20;
    bool is_do_now;
    uint32 req_byte_to_byte;
    uint32 req_wait_time;
    uint32 res_wait_time;
    uint32 res_byte_to_byte;
    uint32 next_address;
    // only for w80

    v1_shared();
};

typedef boost::shared_ptr<v1_shared> v1_shared_ptr;

}
}


#endif	/* JM_V1_SHARED_HPP */

