#include "jmlog.h"
#include "jmauth.h"

#ifdef _MSC_VER
#include <Windows.h>
#endif

static GIOChannel *log_file = NULL;

#define JM_LOG_DOMAIN "JMLib"

static void _jm_log_debug_handler(const gchar   *log_domain,
    GLogLevelFlags log_level,
    const gchar   *message,
    gpointer       user_data)
{
#ifdef _MSC_VER
    {
        gunichar2* uni_msg = g_utf8_to_utf16(message, -1, NULL, NULL, 
            NULL);
        OutputDebugStringW((LPCWSTR)uni_msg);
        g_free(uni_msg);
    }
#endif
    {
        gsize bytes_written;
        g_io_channel_write_chars(log_file, message, 
            g_utf8_strlen(message, -1), &bytes_written, NULL);
        g_io_channel_flush(log_file, NULL);
    }
}

void jm_log_init(void)
{
    g_log_set_handler(JM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
        _jm_log_debug_handler, NULL);
    log_file = g_io_channel_new_file("./log", "a", NULL);
}

void jm_log_destroy(void)
{
    g_io_channel_shutdown(log_file, TRUE, NULL);
    g_io_channel_unref(log_file);
}

static void _jm_log_write(JMLogLevel l, const gchar *str)
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

void jm_log_write(JMLogLevel l, const gchar *tag, const gchar *msg)
{
#ifdef G_PLATFORM_ANDROID
    int priod = l;
    __android_log_write(priod, tag, msg);
#else
    GTimeVal time;
    gchar *str = NULL;
    GString *temp = NULL;

    g_get_current_time(&time);
    str = g_time_val_to_iso8601(&time);
    temp = g_string_new(str);
    g_string_append_printf(temp, "--%s::%s\n", tag, msg);
    _jm_log_write(l, temp->str);
    g_string_free(temp, TRUE);
    g_free(str);
#endif
}

void jm_log_write_hex(JMLogLevel l, const gchar *tag, const guint8 *data, 
    size_t count)
{
    GString *str = g_string_new("");
    size_t pos;
    for (pos = 0; pos < count; pos++)
    {
        g_string_append_printf(str, "%02X ", data[pos]);
    }
    jm_log_write(l, tag, str->str);
    g_string_free(str, TRUE);
}
