#ifndef __JM_DIAG_V1_BOX_VERSION_HPP__
#define __JM_DIAG_V1_BOX_VERSION_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <hash_map>
#include <jm/diag/boxversion.hpp>
#include <jm/diag/v1/c168/box.hpp>
#include <jm/diag/v1/w80/box.hpp>
#include <jm/diag/v1/kwp1281link.hpp>
#include <jm/diag/v1/iso14230.hpp>
#include <jm/diag/v1/iso15765.hpp>
#include <jm/diag/v1/mikuni.hpp>
#include <jm/diag/v1/besturnkwp.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
class BoxVersion : public Diag::BoxVersion
{
public:
    BoxVersion();
    ~BoxVersion();
    void open(boost::system::error_code &ec);
    void close(boost::system::error_code &ec);
    Diag::BoxLink* configure(ProtocolType type);
    void setConnector(Connector cnn);

private:
    template<typename BoxType>
    boost::shared_ptr<Diag::BoxLink> makeProtocol(const boost::shared_ptr<BoxType> &box,
            ProtocolType type)
    {
        boost::shared_ptr<Diag::BoxLink> ret;
        switch (type)
        {
        case PRC_ISO14230:
            ret.reset(new BoxLink<BoxType, ISO14230<BoxType> > (box, _shared));
            break;
        case PRC_ISO15765:
            ret.reset(new BoxLink<BoxType, ISO15765<BoxType> > (box, _shared));
            break;
        case PRC_KWP1281:
            ret.reset(new BoxLink<BoxType, KWP1281<BoxType> > (box, _shared));
            break;
        case PRC_MIKUNI:
            ret.reset(new BoxLink<BoxType, Mikuni<BoxType> > (box, _shared));
            break;
        case PRC_BESTURN_KWP:
            ret.reset(new BoxLink<BoxType, BesturnKWP<BoxType> > (box, _shared));
            break;
        default:
            break;
        }
        return ret;
    }
private:
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<C168::Box> _c168;
    boost::shared_ptr<W80::Box> _w80;
    void *_currentBox;
    std::hash_map<ProtocolType, boost::shared_ptr<Diag::BoxLink> > _w80PrcHash;
    std::hash_map<ProtocolType, boost::shared_ptr<Diag::BoxLink> > _c168PrcHash;
};
}
}
}

#endif