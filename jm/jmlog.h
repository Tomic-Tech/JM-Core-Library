#ifndef __JM_LOG_H__
#define __JM_LOG_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMLogLevel JMLogLevel;

enum _JMLogLevel {
#ifdef G_PLATFORM_ANDROID
	JM_LOG_UNKNOW = ANDROID_LOG_UNKNOWN,
	JM_LOG_DEFAULT = ANDROID_LOG_DEFAULT,
	JM_LOG_VERBOSE = ANDROID_LOG_VERBOSE,
	JM_LOG_DEBUG = ANDROID_LOG_DEBUG,
	JM_LOG_INFO = ANDROID_LOG_INFO,
	JM_LOG_WARN = ANDROID_LOG_WARN,
	JM_LOG_ERROR = ANDROID_LOG_ERROR,
	JM_LOG_FATAL = ANDROID_LOG_FATAL,
	JM_LOG_SILENT = ANDROID_LOG_SILENT
#else
	JM_LOG_UNKNOW,
	JM_LOG_DEFAULT,
	JM_LOG_VERBOSE,
	JM_LOG_DEBUG,
	JM_LOG_INFO,
	JM_LOG_WARN,
	JM_LOG_ERROR,
	JM_LOG_FATAL,
	JM_LOG_SILENT
#endif
};

void jm_log_init(void);
void jm_log_destroy(void);
GLIB_VAR void jm_log_write(JMLogLevel l, const gchar *tag, const gchar *msg);
GLIB_VAR void jm_log_write_hex(JMLogLevel l, const gchar *tag, const guint8 *data, size_t count);

G_END_DECLS

#endif