#ifndef __JM_COMMBOX_HPP__
#define __JM_COMMBOX_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmconnector.h>
#include <boost/cstdint.hpp>
#include <jm/jmlink.hpp>

namespace JM
{
    class Commbox
    {
    public:
        virtual boost::int32_t open() = 0;
        virtual boost::int32_t close() = 0;
        virtual Link* configure(JMProtocolType type) = 0;
        virtual boost::int32_t setConnector(JMConnector cnn) = 0;
    };
}

#endif