#ifndef __JM_DIAG_V1_SHARED_HPP__
#define __JM_DIAG_V1_SHARED_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/cstdint.hpp>
#include <jm/diag/connector.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
struct Shared
{
    Connector connector;
    boost::uint32_t lastError; /* �ṩ�����ѯ */
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
}

#endif