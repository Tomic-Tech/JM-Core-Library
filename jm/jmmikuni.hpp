#ifndef __JM_MIKUNI_HPP__
#define __JM_MIKUNI_HPP__

#include <glib.h>
#include <string.h>

namespace JM
{
    class Mikuni
    {
    public:
        static const guint8 HEAD_FORMAT = 0x48;
    public:
        Mikuni()
        {

        }

        virtual size_t pack(const guint8 *src, size_t count, guint8 *tar)
        {
            if (count <= 0)
                return 0;

            tar[0] = HEAD_FORMAT;
            memcpy(tar + 1, src, count);
            tar[count + 1] = 0x0D;
            tar[count + 2] = 0x0A;
            return count + 3;
        }

        virtual size_t unpack(const guint8 *src, size_t count, guint8 *tar)
        {
            if (count <= 0)
                return 0;

            memcpy(tar, src + 1, count - 3);
            return count - 3;
        }

        virtual gint32 init() = 0;
    };
}

#endif