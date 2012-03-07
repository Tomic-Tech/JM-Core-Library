#ifndef __JM_SERIAL_PORT_PRIVATE_H__
#define __JM_SERIAL_PORT_PRIVATE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmstringarray.h>
#include <jm/jmerror.h>
#include <jm/jmserialport.h>

#ifdef G_OS_WIN32
#include <Windows.h>
#include <ObjBase.h>
#include <InitGuid.h>
#include <SetupAPI.h>
#include <Dbt.h>
#else
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#endif

struct _JMSerialPort
{
	// Data
	gchar* port_name;
	gint64 read_timeout;
	gint64 write_timeout;
	gint32 parity;
	gint32 stopbits;
	gint32 flow_control;
	gint32 baudrate;
	gboolean is_multi_setting;
	guint8 databits;
    GStaticRecMutex *mutex;

#ifdef G_OS_WIN32
	gint64 bytes_to_write;
	HANDLE handle;
	OVERLAPPED read_overlap;
	OVERLAPPED write_overlap;
	COMMCONFIG comm_config;
	COMMTIMEOUTS comm_timeouts;
	DWORD event_mask;
#else
	int fd;
	struct termios comm_config;
	struct termios old_termios;
	struct timeval timeout;
	struct timeval copy_timeout;
#endif
};

void _jm_serial_port_platform_initialize(JMSerialPort *priv);
void _jm_serial_port_platform_destroy(JMSerialPort *priv);

#ifdef G_OS_WIN32
gint32 _jm_serial_port_full_name_win(const gchar* name, gchar **result);
JMStringArray* _jm_serial_port_enumerate_device_win(const GUID *guid);
gchar* _jm_serial_port_get_reg_key_value(HKEY key, const gchar *property);
#else
gint32 _jm_serial_port_set_baudrate(JMSerialPort *priv, tcflag_t baud);
gint32 _jm_serial_port_set_databits(JMSerialPort *priv, tcflag_t bits);
#endif

#endif