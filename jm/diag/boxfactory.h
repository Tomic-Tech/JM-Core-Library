#ifndef __JM_DIAG_BOXFACTORY_H__
#define __JM_DIAG_BOXFACTORY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/commbox.h>

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
    static BoxFactory &inst();
    JMCORE_API void set_version(Version ver);
    JMCORE_API CommboxPtr get_box();
private:
    BoxFactory();
    ~BoxFactory();
private:
    std::map<Version, CommboxPtr> _boxHash;
    Version _ver;
};
}
}

#endif /* __JM_DIAG_BOXFACTORY_H__ */
