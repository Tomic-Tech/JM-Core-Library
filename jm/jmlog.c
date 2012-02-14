#include "jmlog.h"

static void _jm_log_write(JMLogLevel l) {
	switch(l) {
	case JM_LOG_UNKNOW:
		g_print("Unknow: ");
		break;
	case JM_LOG_DEFAULT:
		g_print("Default: ");
		break;
	case JM_LOG_VERBOSE:
		g_print("Verbose: ");
		break;
	case JM_LOG_DEBUG:
		g_print("Debug: ");
		break;
	case JM_LOG_INFO:
		g_print("Info: ");
		break;
	case JM_LOG_WARN:
		g_print("Warn: ");
		break;
	case JM_LOG_ERROR:
		g_print("Error: ");
		break;
	case JM_LOG_FATAL:
		g_print("Fatal: ");
		break;
	case JM_LOG_SILENT:
		g_print("Silent: ");
		break;
	}
}

void jm_log_write(JMLogLevel l, const gchar *tag, const gchar *msg) {
#ifdef G_PLATFORM_ANDROID
	int priod = l;
	__android_log_write(priod, tag, msg);
#else
	GTimeVal time;
	gchar *str = NULL;
	g_get_current_time(&time);
	str = g_time_val_to_iso8601(&time);
	_jm_log_write(l);
	g_print(str);
	g_print("--%s::%s\n", tag, msg);
	g_free(str);
#endif
}

void jm_log_write_hex(JMLogLevel l, const gchar *tag, const guint8 *data, size_t count) {
	GString *str = g_string_new("");
	size_t pos;
	for (pos = 0; pos < count; pos++) {
		g_string_append_printf(str, "%02X ", data[pos]);
	}
	jm_log_write(l, tag, str->str);
	g_string_free(str, TRUE);
}
