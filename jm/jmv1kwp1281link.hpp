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
        template<typename BoxType>
        class KWP1281Link : public Link<BoxType, KWP1281<BoxType> >
        {
        public:
            KWP1281Link(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &shared)
                : Link(box, shared)
            {

            }
            std::size_t sendAndRecv(const boost::uint8_t *send, std::size_t count, boost::uint8_t *recv, std::size_t maxLength)
            {
                std::size_t length = sendOneFrame(send, count);
                if (length != count)
                    return 0;
                return readOneFrame(recv, maxLength);
            }
        };
    }
}

#endif