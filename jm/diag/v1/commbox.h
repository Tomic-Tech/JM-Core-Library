#ifndef __JM_DIAG_V1_COMMBOX_H__
#define __JM_DIAG_V1_COMMBOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/commbox.h>
#include <jm/diag/v1/c168/box.h>
#include <jm/diag/v1/w80/box.h>
#include <jm/diag/v1/kwp1281.h>
#include <jm/diag/v1/iso14230.h>
#include <jm/diag/v1/iso15765.h>
#include <jm/diag/v1/mikuni.h>
#include <jm/diag/v1/besturnkwp.h>

namespace JM
{
namespace Diag
{
namespace V1
{

class Commbox : public JM::Diag::Commbox
{
public:
    Commbox();
    ~Commbox();
    boost::system::error_code open(boost::system::error_code &ec);
    boost::system::error_code close(boost::system::error_code &ec);
    boost::system::error_code set_connector(Connector enn, boost::system::error_code &ec);
    void * make_protocol(ProtocolType type);
private:
    template<typename BoxType>
    void * make_protocol(ProtocolType type, boost::shared_ptr<BoxType> &box)
    {        
        switch(type)
        {
        case PRC_ISO14230:
            return new ISO14230<BoxType>(box, _shared);
            break;
        case PRC_ISO15765:
            return new ISO15765<BoxType>(box, _shared);
            break;
        case PRC_KWP1281:
            return new V1::KWP1281<BoxType>(box, _shared);
            break;
        case PRC_MIKUNI:
            return new V1::Mikuni<BoxType>(box, _shared);
            break;
        case PRC_BESTURN_KWP:
            return new V1::BesturnKWP<BoxType>(box, _shared);
            break;
        default:
            return NULL;
        }
    }
private:
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<C168::Box> _c168;
    boost::shared_ptr<w80::Box> _w80;
    void * _currentBox;
    std::map<ProtocolType, boost::shared_ptr<Diag::Link> > _w80PrcHash;
    std::map<ProtocolType, boost::shared_ptr<Diag::Link> > _c168PrcHash;
};

}
}
}

#endif /* __JM_DIAG_V1_COMMBOX_H__ */