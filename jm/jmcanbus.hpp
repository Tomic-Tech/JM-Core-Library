#ifndef __JM_CANBUS_HPP__
#define __JM_CANBUS_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

namespace JM
{
    class Canbus
    {
    public:
        Canbus()
        {
            _flowControl[0] = 0x30;
            _flowControl[1] = 0x00;
            _flowControl[2] = 0x00;
            _flowControl[3] = 0x00;
            _flowControl[4] = 0x00;
            _flowControl[5] = 0x00;
            _flowControl[6] = 0x00;
            _flowControl[7] = 0x00;
            _idArray = g_ptr_array_new();
        }

        virtual ~Canbus()
        {
            g_ptr_array_free(_idArray, TRUE);
        }

        virtual size_t pack(const guint8 *src, size_t count, guint8 *tar)
        {
            if (count > 8 || count <= 0)
            {
                return 0;
            }

            if (_idMode == JM_CANBUS_ID_MODE_STD)
            {
                tar[1] = JM_HIGH_BYTE(JM_LOW_WORD(_targetID));
                tar[2] = JM_LOW_BYTE(JM_LOW_WORD(_targetID));
                if (_frameType == JM_CANBUS_FRAME_TYPE_DATA)
                {
                    tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_STD | 
                        JM_CANBUS_FRAME_TYPE_DATA);
                }
                else
                {
                    tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_STD | 
                        JM_CANBUS_FRAME_TYPE_REMOTE);
                }
                memcpy(tar + 3, src, count);
                return count + 3;
            }
            if (_idMode == JM_CANBUS_ID_MODE_EXT)
            {
                tar[1] = JM_HIGH_BYTE(JM_HIGH_WORD(_targetID));
                tar[2] = JM_LOW_BYTE(JM_HIGH_WORD(_targetID));
                tar[3] = JM_HIGH_BYTE(JM_LOW_WORD(_targetID));
                tar[4] = JM_LOW_BYTE(JM_LOW_WORD(_targetID));
                if (_frameType == JM_CANBUS_FRAME_TYPE_DATA)
                {
                    tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_EXT |
                        JM_CANBUS_FRAME_TYPE_DATA);
                }
                else
                {
                    tar[0] = JM_LOW_BYTE(count | JM_CANBUS_ID_MODE_EXT | 
                        JM_CANBUS_FRAME_TYPE_REMOTE);
                }
                memcpy(tar + 5, src, count);
                return count + 5;
            }
            return 0;
        }

        virtual size_t unpack(const guint8 *src, size_t count, guint8 *tar)
        {
            guint32 mode;
            size_t length = 0;

            if (count <= 0)
            {
                return 0;
            }

            mode = (src[0] & (JM_CANBUS_ID_MODE_EXT | 
                JM_CANBUS_FRAME_TYPE_REMOTE));
            if (mode == (JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA))
            {
                length = src[0] & 0x0F;
                if (length != count - 3)
                {
                    return 0;
                }
                memcpy(tar, src + 3, length);
                return length;
            }

            if (mode == (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA))
            {
                length = src[0] & 0x0F;
                if (length != count - 5)
                {
                    return 0;
                }
                memcpy(tar, src + 5, length);
            }
            return length;
        }

        virtual gint32 setLines(gint32 high, gint32 low)
        {
            _high = high;
            _low = low;

            return JM_ERROR_SUCCESS;
        }

        virtual gint32 setFilter(const gint32 *idArray, size_t count)
        {
            size_t i;
            g_ptr_array_set_size(_idArray, 0);

            for (i = 0; i < count; i++)
            {
                g_ptr_array_add(_idArray, GINT_TO_POINTER(idArray[i]));
            }
            return JM_ERROR_SUCCESS;
        }
        virtual gint32 setOptions(gint32 id, JMCanbusBaud baud,
            JMCanbusIDMode idMode,
            JMCanbusFilterMask mask,
            JMCanbusFrameType frame)
        {
            _baud = baud;
            _idMode = idMode;
            _filterMask = mask;
            _frameType = frame;
            _targetID = id;
            return JM_ERROR_SUCCESS;
        }
    protected:
        gint32 _targetID;
        JMCanbusBaud _baud;
        JMCanbusIDMode _idMode;
        JMCanbusFilterMask _filterMask;
        JMCanbusFrameType _frameType;
        gint32 _high;
        gint32 _low;
        GPtrArray *_idArray;
        guint8 _flowControl[8];
    };
}

#endif