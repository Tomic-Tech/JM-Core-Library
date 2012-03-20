#ifndef __JM_V1_KWP1281_LINK_HPP__
#define __JM_V1_KWP1281_LINK_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmv1link.hpp"
#include "jmv1kwp1281.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BOX>
        class KWP1281Link : public Link<BOX, KWP1281<BOX> >
        {
        public:
            KWP1281Link(BOX *box, Shared *shared)
                : Link(box, shared)
            {

            }
            size_t sendAndRecv(const guint8 *send, size_t count, guint8 *recv)
            {
                size_t length = sendOneFrame(send, count);
                if (length != count)
                    return 0;
                return readOneFrame(recv);
            }
        };
    }
}

#endif