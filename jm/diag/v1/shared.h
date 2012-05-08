#ifndef __JM_DIAG_V1_SHARED_H__
#define __JM_DIAG_V1_SHARED_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/connector.h>

namespace JM
{
namespace Diag
{
namespace V1
{

struct Shared
{
    Connector cnn;
    boost::uint32_t lastError;
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

#endif /* __JM_DIAG_V1_SHARED_H__ */
