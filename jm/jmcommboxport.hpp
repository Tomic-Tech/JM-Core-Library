#ifndef __JM_COMMBOX_PORT_HPP__
#define __JM_COMMBOX_PORT_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmserialport.hpp"

namespace JM
{
    inline void freeOutDeque(gpointer data, gpointer user_data)
    {
        GByteArray *array = (GByteArray*)data;
        g_byte_array_free(array, TRUE);
    }

    class CommboxPort
    {
    public:
        static CommboxPort& inst()
        {
            static CommboxPort instance;
            return instance;
        }

        void setType(JMCommboxPortType type)
        {
            _portType = type;
        }

        JMCommboxPortType type()
        {
            return _portType;
        }

        void setPointer(gpointer p)
        {
            if (_portType == JM_COMMBOX_PORT_SERIAL_PORT)
            {
                _pointer = ((JMSerialPort*)p)->_handle;
            }
            else
            {
                _pointer = p;
            }
        }

        gpointer pointer()
        {
            return _pointer;
        }

        gint32 setReadTimeout(gint64 microseconds)
        {
            _readTimeout = microseconds;
            return JM_ERROR_SUCCESS;
        }

        gint32 setWriteTimeout(gint64 microseconds)
        {
            _writeTimeout = microseconds;
            return JM_ERROR_SUCCESS;
        }

        size_t bytesAvailable()
        {
            return g_queue_get_length(_inDeque);
        }

        gint32 discardOutBuffer()
        {
            g_mutex_lock(_outMutex);
            g_queue_foreach(_outDeque, freeOutDeque, NULL);
            g_queue_clear(_outDeque);
            g_mutex_unlock(_outMutex);
            return JM_ERROR_SUCCESS;
        }

        gint32 discardInBuffer()
        {
            g_mutex_lock(_inMutex);
            g_queue_clear(_inDeque);
            g_mutex_unlock(_inMutex);
            return JM_ERROR_SUCCESS;
        }

        size_t read(guint8 *data, size_t count)
        {
            if (count <= g_queue_get_length(_inDeque))
                return readImmediately(data, count);

            if (_readTimeout <= 0)
                return readWithoutTimeout(data, count);

            return readWithTimeout(data, count);
        }

        size_t write(const guint8 *data, size_t count)
        {
            GByteArray *array = NULL;
            g_mutex_lock(_outMutex);
            array = g_byte_array_sized_new(count);
            array = g_byte_array_append(array, data, count);
            g_queue_push_tail(_outDeque, array);
            g_mutex_unlock(_outMutex);
            return count;
        }

        void pushInDeque(const guint8 *data, size_t count)
        {
            size_t i;
            g_mutex_lock(_inMutex);
            for (i = 0; i < count; i++)
            {
                g_queue_push_tail(_inDeque, GUINT_TO_POINTER(data[i]));
            }
            g_cond_signal(_inCond);
            g_mutex_unlock(_inMutex);
        }

        gboolean outDequeAvailable()
        {
            return !g_queue_is_empty(_outDeque);
        }

        gboolean popOutDeque(GByteArray **data)
        {
            g_mutex_lock(_outMutex);
            if (g_queue_is_empty(_outDeque))
                return FALSE;
            if (*data != NULL)
                g_byte_array_free(*data, TRUE);
            *data = (GByteArray*)g_queue_pop_head(_outDeque);
            g_mutex_unlock(_outMutex);
            return TRUE;
        }
    private:
        CommboxPort()
            : _readTimeout(500000)
            , _writeTimeout(500000)
            , _inDeque(g_queue_new())
            , _outDeque(g_queue_new())
            , _inMutex(g_mutex_new())
            , _outMutex(g_mutex_new())
            , _inCond(g_cond_new())
            , _portType(JM_COMMBOX_PORT_UNKNOW)
        {
        }

        ~CommboxPort()
        {
            g_queue_free(_inDeque);
            g_queue_foreach(_outDeque, freeOutDeque, NULL);
            g_queue_free(_outDeque);
            g_mutex_free(_inMutex);
            g_mutex_free(_outMutex);
            g_cond_free(_inCond);
        }

        size_t readImmediately(guint8 *data, size_t count)
        {
            size_t i;
            g_mutex_lock(_inMutex);
            for (i = 0; i < count; i++)
            {
                data[i] = (guint8)GPOINTER_TO_UINT(g_queue_pop_head(_inDeque));
            }
            g_mutex_unlock(_inMutex);
            return count;
        }

        size_t readWithoutTimeout(guint8 *data, size_t count)
        {
            size_t i;
            g_mutex_lock(_inMutex);
            while (g_queue_get_length(_inDeque) < count)
            {
                g_cond_wait(_inCond, _inMutex);
            }
            for (i = 0; i < count; i++)
            {
                data[i] = (guint8)GPOINTER_TO_UINT(g_queue_pop_head(_inDeque));
            }
            g_mutex_unlock(_inMutex);
            return count;
        }

        size_t readWithTimeout(guint8 *data, size_t count)
        {
            size_t i;
            size_t len = 0;
            GTimeVal abs_time;

            g_mutex_lock(_inMutex);

            g_get_current_time(&abs_time);
            g_time_val_add(&abs_time, _readTimeout);

            while (g_cond_timed_wait(_inCond, _inMutex, &abs_time))
            {
                size_t size = g_queue_get_length(_inDeque);
                if (size >= count)
                {
                    for (i = 0; i < count; i++)
                    {
                        data[i + len] = (guint8)GPOINTER_TO_UINT(
                            g_queue_pop_head(_inDeque));
                    }
                    len += count;
                    break;
                }
                for (i = 0; i < size; i++)
                {
                    data[i + len] = (guint8)GPOINTER_TO_UINT(
                        g_queue_pop_head(_inDeque));
                }
                len += size;
                count -= size;
            }
            g_mutex_unlock(_inMutex);
            return len;
        }
    private:
        gint64 _readTimeout;
        gint64 _writeTimeout;
        GQueue *_inDeque;
        GQueue *_outDeque;
        GMutex *_inMutex;
        GMutex *_outMutex;
        GCond *_inCond;
        JMCommboxPortType _portType;
        gpointer _pointer;
    };
}

#endif