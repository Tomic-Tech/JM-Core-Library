#ifndef __JM_DIAG_BOX_VERSION_HPP__
#define __JM_DIAG_BOX_VERSION_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <jm/types.hpp>
#include <jm/diag/protocoltype.hpp>
#include <jm/diag/connector.hpp>
#include <jm/diag/commbox.hpp>
#include <jm/diag/boxlink.hpp>

namespace JM
{
namespace Diag
{
class BoxVersion
{
public:
    virtual JMCORE_API void open(boost::system::error_code &ec) = 0;
    virtual JMCORE_API void close(boost::system::error_code &ec) = 0;
    virtual JMCORE_API BoxLink* configure(ProtocolType type) = 0;
    virtual JMCORE_API void setConnector(Connector cnn) = 0;
};

typedef boost::shared_ptr<BoxVersion> BoxVersionPtr;
}
}

#endif