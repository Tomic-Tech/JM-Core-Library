#ifndef __JM_SERIAL_PORT_HPP_
#define __JM_SERIAL_PORT_HPP_

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmstringarray.h"
#include "jmerror.h"
#include "jmserialport.h"

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

namespace JM
{
    class SerialPort
    {
    public:
        SerialPort();
        SerialPort(const gchar *name);
        virtual ~SerialPort();
        const gchar* portName();
        gint32 setPortName(const gchar *name);
        gint32 baudrate();
        gint32 setBaudrate(gint32 baudrate);
        guint8 databits();
        gint32 setDatabits(guint8 databits);
        gint32 parity();
        gint32 setParity(gint32 parity);
        gint32 stopbits();
        gint32 setStopbits(gint32 stopbits);
        gint32 flowControl();
        gint32 setFlowControl(gint32 flowControl);
        size_t write(const guint8 *data, size_t count);
        size_t read(guint8 *data, size_t count);
        gint64 readTimeout();
        gint32 setReadTimeout(gint64 millic);
        gint64 writeTimeout();
        gint32 setWriteTimeout(gint64 millic);
        gboolean isOpen();
        gint32 open();
        gint32 close();
        gint32 flush();
        gint32 discardInBuffer();
        gint32 discardOutBuffer();
        size_t bytesAvailable();
        gint32 setDtr(gboolean set);
        gint32 setRts(gboolean set);
        static JMStringArray* getSystemPorts();
    private:
        void init();
        void platformInit();
        void platformDestroy();
#ifdef G_OS_WIN32
        static gint32 fullNameWin(const gchar* name, gchar **result);
        static JMStringArray* enumerateDeviceWin(const GUID *guid);
        static gchar* getRegKeyValue(HKEY key, const gchar *property);
#else
        gint32 _setBaudrate(tcflag_t baud);
        gint32 _setDatabits(tcflag_t bits);
#endif
    private:
        // Data
        gchar* _portName;
        gint64 _readTimeout;
        gint64 _writeTimeout;
        gint32 _parity;
        gint32 _stopbits;
        gint32 _flowControl;
        gint32 _baudrate;
        gboolean _isMultiSetting;
        guint8 _databits;
        GStaticRecMutex *_mutex;
#ifdef G_OS_WIN32
        gint64 _bytesToWrite;
        HANDLE _handle;
        OVERLAPPED _readOverlap;
        OVERLAPPED _writeOverlap;
        COMMCONFIG _commConfig;
        COMMTIMEOUTS _commTimeouts;
        DWORD _eventMask;
#else
        int fd;
        struct termios _commConfig;
        struct termios _oldTermios;
        struct timeval _timeout;
        struct timeval _copyTimeout;
#endif
    };
}

#endif