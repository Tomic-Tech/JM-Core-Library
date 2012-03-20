#ifndef __JM_LOG_HPP__
#define __JM_LOG_HPP__

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include <glib.h>
#include "jmlib.h"

#define JM_LOG_DOMAIN "JMLib"

namespace JM
{
    static void debugHandler(const gchar   *logDomain,
        GLogLevelFlags logLevel,
        const gchar   *message,
        gpointer       userData);

    class Log
    {
    public:
        friend void debugHandler(const gchar *, GLogLevelFlags, const gchar*, gpointer);
        static Log& inst()
        {
            static Log instance;
            return instance;
        }

        void write(JMLogLevel l, const gchar *tag, const gchar *msg)
        {
            GTimeVal time;
            gchar *str = NULL;
            GString *temp = NULL;

            g_get_current_time(&time);
            str = g_time_val_to_iso8601(&time);
            temp = g_string_new(str);
            g_string_append_printf(temp, "--%s::%s\n", tag, msg);
            write(l, temp->str);
            g_string_free(temp, TRUE);
            g_free(str);

        }

        void write(JMLogLevel l, const gchar *tag, const guint8 *data, 
            size_t count)
        {
            GString *str = g_string_new("");
            size_t pos;
            for (pos = 0; pos < count; pos++)
            {
                g_string_append_printf(str, "%02X ", data[pos]);
            }
            write(l, tag, str->str);
            g_string_free(str, TRUE);

        }
    private:
        Log()
            : _file(NULL)
        {
            g_log_set_handler(JM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                debugHandler, NULL);
            _file = g_io_channel_new_file("./log", "a", NULL);
        }

        ~Log()
        {
            g_io_channel_shutdown(_file, TRUE, NULL);
            g_io_channel_unref(_file);
        }

        void write(JMLogLevel l, const gchar *str)
        {
            gchar *log_pw = jm_auth_decrypt_log_pw();

            if (log_pw == NULL)
                return;
            g_free(log_pw);

            switch(l)
            {
            case JM_LOG_UNKNOW:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_MASK, str);
                break;
            case JM_LOG_DEFAULT:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_MASK, str);
                break;
            case JM_LOG_VERBOSE:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, str);
                break;
            case JM_LOG_DEBUG:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, str);
                break;
            case JM_LOG_INFO:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_INFO, str);
                break;
            case JM_LOG_WARN:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, str);
                break;
            case JM_LOG_ERROR:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, str);
                break;
            case JM_LOG_FATAL:
                g_log(JM_LOG_DOMAIN, G_LOG_FLAG_FATAL, str);
                break;
            case JM_LOG_SILENT:
                g_log(JM_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, str);
                break;
            }
        }
    private:
        GIOChannel *_file;
    };

    static void debugHandler(const gchar   *logDomain,
        GLogLevelFlags logLevel,
        const gchar   *message,
        gpointer       userData)
    {
        JM::Log *obj = (JM::Log*)userData;
#ifdef _MSC_VER
        {
            gunichar2* uni_msg = g_utf8_to_utf16(message, -1, NULL, NULL, 
                NULL);
            OutputDebugStringW((LPCWSTR)uni_msg);
            g_free(uni_msg);
        }
#elif defined(G_PLATFORM_ANDROID)
        int priod = l;
        __android_log_write(priod, JM_LOG_DOMAIN, msg);
#else


#endif
        {
            gsize bytes_written;
            g_io_channel_write_chars(obj->_file, message, 
                g_utf8_strlen(message, -1), &bytes_written, NULL);
            g_io_channel_flush(obj->_file, NULL);
        }
    }
}

#endif