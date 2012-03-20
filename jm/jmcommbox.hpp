#ifndef __JM_COMMBOX_HPP__
#define __JM_COMMBOX_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmlink.hpp>

namespace JM
{
    class Commbox
    {
    public:
        virtual gint32 open() = 0;
        virtual gint32 close() = 0;
        virtual Link* configure(JMProtocolType type) = 0;
        virtual gint32 setConnector(JMConnector cnn) = 0;
    };
}

#endif