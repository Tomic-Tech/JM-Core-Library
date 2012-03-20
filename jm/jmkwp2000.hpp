#ifndef __JM_KWP2000_HPP__
#define __JM_KWP2000_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

namespace JM
{
    class KWP2000
    {
    public:
        static const guint8 KWP8X_HEADER_LENGTH = 3;
        static const guint8 KWPCX_HEADER_LENGTH = 3;
        static const guint8 KWP80_HEADER_LENGTH = 4;
        static const guint8 KWPXX_HEADER_LENGTH = 1;
        static const guint8 KWP00_HEADER_LENGTH = 2;
        static const guint8 KWP_CHECKSUM_LENGTH = 1;
        static const guint16 KWP_MAX_DATA_LENGTH = 128;
    public:
        KWP2000()
            : _mode(JM_KWP8X)
            , _linkMode(JM_KWP8X)
            , _msgMode(JM_KWP8X)
            , _baud(10416)
            , _targetAddress(0x12)
            , _sourceAddress(0xF1)
        {

        }
        
        virtual gint32 setOptions(JMKWPMode msgMode, JMKWPMode linkMode, gint32 baud)
        {
            _msgMode = msgMode;
            _linkMode = linkMode;
            _mode = msgMode;
            _baud = baud;

            return JM_ERROR_SUCCESS;
        }

        virtual gint32 setAddress(guint8 targetAddress, guint8 sourceAddress)
        {
            _targetAddress = targetAddress;
            _sourceAddress = sourceAddress;

            return JM_ERROR_SUCCESS;
        }

        virtual size_t pack(const guint8 *src, size_t count, guint8 *tar)
        {
            size_t pos = 0;
            guint8 checksum = 0;
            size_t i = 0;

            g_return_val_if_fail(count > 0, 0);

            if (_mode == JM_KWP8X)
            {
                tar[pos++] = JM_LOW_BYTE(0x80 | count);
                tar[pos++] = JM_LOW_BYTE(_targetAddress);
                tar[pos++] = JM_LOW_BYTE(_sourceAddress);
                memcpy(tar + pos, src, count);
                pos += count;
            }
            else if (_mode == JM_KWPCX)
            {
                tar[pos++] = JM_LOW_BYTE(0xC0 | count);
                tar[pos++] = JM_LOW_BYTE(_targetAddress);
                tar[pos++] = JM_LOW_BYTE(_sourceAddress);
                memcpy(tar + pos, src, count);
                pos += count;
            }
            else if (_mode == JM_KWP80)
            {
                tar[pos++] = JM_LOW_BYTE(0x80);
                tar[pos++] = JM_LOW_BYTE(_targetAddress);
                tar[pos++] = JM_LOW_BYTE(_sourceAddress);
                tar[pos++] = JM_LOW_BYTE(count);
                memcpy(tar + pos, src, count);
                pos += count;
            }
            else if (_mode == JM_KWP00)
            {
                tar[pos++] = 0x00;
                tar[pos++] = JM_LOW_BYTE(count);
                memcpy(tar + pos, src, count);
                pos += count;
            }
            else if (_mode == JM_KWPXX)
            {
                tar[pos++] = JM_LOW_BYTE(count);
                memcpy(tar + pos, src, count);
                pos += count;
            }

            for (i = 0; i < pos; i++)
            {
                checksum += tar[i];
            }
            tar[pos++] = checksum;
            return pos;

        }

        size_t unpack(const guint8 *src, size_t count, guint8 *tar)
        {
            size_t length = 0;
            g_return_val_if_fail(count > 0, 0);

            if (src[0] > 0x80)
            {
                length = src[0] - 0x80;
                if (src[1] != _sourceAddress)
                {
                    return 0;
                }
                if (length != (count - KWP8X_HEADER_LENGTH - 
                    KWP_CHECKSUM_LENGTH))
                {
                    length = src[0] - 0xC0; /* For KWPCX */
                    if (length != (count - KWPCX_HEADER_LENGTH - 
                        KWP_CHECKSUM_LENGTH))
                    {
                        return 0;
                    }
                }
                memcpy(tar, src + KWP8X_HEADER_LENGTH, length);
            }
            else if (src[0] == 0x80)
            {
                length = src[0];
                if (src[1] != _sourceAddress)
                {
                    return 0;
                }

                if (length != (count - KWP80_HEADER_LENGTH - 
                    KWP_CHECKSUM_LENGTH))
                {
                    return 0;
                }
                memcpy(tar, src + KWP80_HEADER_LENGTH, length);
            }
            else if (src[0] == 0x00)
            {
                length = src[1];
                if (length != (count - KWP00_HEADER_LENGTH - 
                    KWP_CHECKSUM_LENGTH))
                {
                    return 0;
                }
                memcpy(tar, src + KWP00_HEADER_LENGTH, length);
            }
            else
            {
                length = src[0];
                if (length != (count - KWP00_HEADER_LENGTH - 
                    KWP_CHECKSUM_LENGTH))
                {
                    return 0;
                }
                memcpy(tar, src + KWP00_HEADER_LENGTH, length);
            }
            return length;
        }

        virtual gint32 addrInit(guint8 addrCode) = 0;
        virtual gint32 fastInit(const guint8 *data, size_t count) = 0;
        virtual gint32 setLines(gint32 comLine, gboolean lLine) = 0;
    protected:
        JMKWPMode _mode;
        JMKWPMode _linkMode;
        JMKWPMode _msgMode;
        gint32 _baud;
        guint32 _targetAddress;
        guint32 _sourceAddress;

    };
}

#endif