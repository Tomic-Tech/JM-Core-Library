#ifndef __JM_V1_SHARED_HPP__
#define __JM_V1_SHARED_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmconnector.h"
#include <boost/cstdint.hpp>

namespace JM
{
    namespace V1
    {
        class Shared
        {
        public:
            JMConnector connector;
            boost::uint32_t lastError; /* 提供错误查询 */
            bool isDB20;
            bool isDoNow;
            boost::int64_t reqByteToByte;
            boost::int64_t reqWaitTime;
            boost::int64_t resWaitTime;
            boost::int64_t resByteToByte;
            boost::uint32_t nextAddress;
            boost::uint8_t buffID;
        };
    }
}

#endif
