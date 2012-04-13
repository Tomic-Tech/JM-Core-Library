#ifndef __JM_DIAG_BOX_FACTORY_HPP__
#define __JM_DIAG_BOX_FACTORY_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <hash_map>
#include <boost/smart_ptr.hpp>
#include <jm/diag/commbox.hpp>
#include <jm/diag/boxversion.hpp>

namespace JM
{
namespace Diag
{
class BoxFactory
{
public:
    enum Version
    {
        V1,
        ELM327,
        TL718,
        V2
    };
public:
    static BoxFactory& inst();
    JMCORE_API void setVersion(Version ver);
    JMCORE_API BoxVersionPtr getBox();
private:
    BoxFactory();
    ~BoxFactory();
private:
    std::hash_map<Version, BoxVersionPtr > _boxHash;
    Version _ver;
};

}
}


#endif