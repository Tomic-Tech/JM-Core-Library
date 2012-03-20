#ifndef __JM_KWP1281_HPP__
#define __JM_KWP1281_HPP__

#include <glib.h>
#include <string.h>
#include <jm/jmutils.h>

namespace JM
{
    class KWP1281
    {
    public:
        static const guint8 FRAME_END = 0x03;
    public:

        KWP1281()
            : _frameCounter(0)
        {

        }

        size_t pack(const guint8 *src, size_t count, guint8 *tar)
        {
            tar[0] = JM_LOW_BYTE(count + 20);
            tar[1] = frameCounterIncrement();

            memcpy(tar + 2, src, count);
            tar[count + 2] = FRAME_END;
            return count + 3;
        }

        size_t unpack(const guint8 *src, size_t count, guint8 *tar)
        {
            memcpy(tar, src + 1, count - 2);
            return count - 2;
        }

        virtual gint32 addrInit(guint8 addrCode) = 0;
        virtual gint32 setLines(gint32 comLine, gboolean lLine) = 0;

    private:
        guint8 frameCounterIncrement()
        {
            return ++(_frameCounter);
        }

    protected:
        guint8 _frameCounter;
    };
}

#endif