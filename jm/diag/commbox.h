#ifndef __JM_DIAG_COMMBOX_H__
#define __JM_DIAG_COMMBOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/connector.h>
#include <jm/diag/canbus.h>
#include <jm/diag/kwp1281.h>
#include <jm/diag/kwp2000.h>
#include <jm/diag/mikuni.h>
#include <jm/diag/protocoltype.h>

namespace JM
{
namespace Diag
{

class Commbox
{
public:
    virtual JMCORE_API boost::system::error_code
    open(boost::system::error_code &ec) = 0;

    virtual JMCORE_API boost::system::error_code
    close(boost::system::error_code &ec) = 0;

    virtual JMCORE_API boost::system::error_code
    set_connector(Connector cnn, boost::system::error_code &ec) = 0;

    template<typename PROTOCOL>
    boost::shared_ptr<PROTOCOL> create_protocol(ProtocolType type)
    {
        boost::shared_ptr<PROTOCOL> p((PROTOCOL *)make_protocol(type));
        return p;
    }
protected:
    virtual void *
    make_protocol(ProtocolType type) = 0;

};

typedef boost::shared_ptr<Commbox> CommboxPtr;

}
}

#endif