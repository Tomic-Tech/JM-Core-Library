#ifndef __JM_LINK_HPP__
#define __JM_LINK_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmprotocoltype.h"

namespace JM
{
    class Link
    {
    public:

        virtual gpointer protocol()
        {
            return _prc;
        }

        void setProtocolType(JMProtocolType type)
        {
            _prcType = type;
        }

        JMProtocolType protocolType()
        {
            return _prcType;
        }

        virtual size_t sendOneFrame(const guint8 *data, size_t count) = 0;
        virtual size_t sendFrames(const guint8 *data, size_t count) = 0;
        virtual size_t readOneFrame(guint8 *data) = 0;
        virtual size_t readFrames(guint8 *data) = 0;
        virtual size_t sendAndRecv(const guint8 *send, size_t sendCount, guint8 *recv) = 0;
        virtual gint32 startKeepLink(gboolean run) = 0;
        virtual gint32 setKeepLink(const guint8 *data, size_t count) = 0;
        virtual gint32 setTimeout(gint32 txB2B, gint32 rxB2B, gint32 txF2F, gint32 rxF2F, gint32 total) = 0;

    private:
        JMProtocolType _prcType;
        gpointer _prc;
    };
}

#endif