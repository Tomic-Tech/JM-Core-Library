#include "jmserialportprivate.h"
#include "jmlog.h"

#ifdef G_OS_WIN32

// see http://msdn.microsoft.com/en-us/library/ms791134.aspx for list of GUID classes
#ifndef GUID_DEVCLASS_PORTS
DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 
    0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
#endif

void _jm_serial_port_platform_initialize(JMSerialPort *self)
{
    self->bytes_to_write = 0;
    self->handle = INVALID_HANDLE_VALUE;
    ZeroMemory(&self->comm_config, sizeof(COMMCONFIG));
    ZeroMemory(&self->comm_timeouts, sizeof(COMMTIMEOUTS));
    ZeroMemory(&self->read_overlap, sizeof(OVERLAPPED));
    ZeroMemory(&self->write_overlap, sizeof(OVERLAPPED));
    self->read_overlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    self->write_overlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    jm_serial_port_set_port_name(self, "\\\\.\\COM1");
}

void _jm_serial_port_platform_destroy(JMSerialPort *self)
{
    g_return_if_fail(self != NULL);
    CloseHandle(self->read_overlap.hEvent);
    CloseHandle(self->write_overlap.hEvent);
}

gint32 jm_serial_port_set_port_name(JMSerialPort *self, const gchar *name)
{
    gint32 ret;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(name != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);
    if (jm_serial_port_is_open(self)) {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    ret = _jm_serial_port_full_name_win(name, &self->port_name);
    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_set_baudrate(JMSerialPort *self, gint32 baudrate)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (self->baudrate != baudrate)
    {
        switch (baudrate)
        {
        case 50:
        case 75:
        case 134:
        case 150:
        case 200:
        case 1800:
        case 76800:
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        default:
            self->baudrate = baudrate;
            break;
        }
    }

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    switch (baudrate)
    {
    case 110:
        self->comm_config.dcb.BaudRate = CBR_110;
        break;
    case 300:
        self->comm_config.dcb.BaudRate = CBR_300;
        break;
    case 600:
        self->comm_config.dcb.BaudRate = CBR_600;
        break;
    case 1200:
        self->comm_config.dcb.BaudRate = CBR_1200;
        break;
    case 2400:
        self->comm_config.dcb.BaudRate = CBR_2400;
        break;
    case 4800:
        self->comm_config.dcb.BaudRate = CBR_4800;
        break;
    case 9600:
        self->comm_config.dcb.BaudRate = CBR_9600;
        break;
    case 14400:
        self->comm_config.dcb.BaudRate = CBR_14400;
        break;
    case 19200:
        self->comm_config.dcb.BaudRate = CBR_19200;
        break;
    case 38400:
        self->comm_config.dcb.BaudRate = CBR_38400;
        break;
    case 57600:
        jm_log_write(JM_LOG_DEBUG, "Serial Port", "Set baud rate 57600");
        self->comm_config.dcb.BaudRate = CBR_57600;
        break;
    case 115200:
        self->comm_config.dcb.BaudRate = CBR_115200;
        break;
    case 128000:
        self->comm_config.dcb.BaudRate = CBR_128000;
        break;
    case 256000:
        self->comm_config.dcb.BaudRate = CBR_256000;
        break;
    default:
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (!SetCommConfig(self->handle, &(self->comm_config), 
        sizeof(COMMCONFIG)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_set_databits(JMSerialPort *self, guint8 databits)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (databits < 5 || databits > 8)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (self->databits != databits)
    {
        if (self->stopbits == JM_SP_SB_TWO && databits == 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE && databits != 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        if (self->parity == JM_SP_PAR_SPACE && databits == 8)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->databits = databits;
    }
    g_static_rec_mutex_unlock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    switch (databits)
    {
    case 5:
        if (self->stopbits == JM_SP_SB_TWO)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->comm_config.dcb.ByteSize = 5;
        break;
    case 6:
        if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->comm_config.dcb.BaudRate = 6;
        break;
    case 7:
        if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->comm_config.dcb.ByteSize = 7;
        break;
    case 8:
        if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->comm_config.dcb.ByteSize = 8;
        break;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (!SetCommConfig(self->handle, &(self->comm_config), 
        sizeof(COMMCONFIG)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_set_parity(JMSerialPort *self, gint32 parity)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!(parity == JM_SP_PAR_NONE || 
        parity == JM_SP_PAR_SPACE || 
        parity == JM_SP_PAR_EVEN || 
        parity == JM_SP_PAR_ODD || 
        parity == JM_SP_PAR_MARK))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (self->parity != parity)
    {
        self->parity = parity;
    }

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    self->comm_config.dcb.Parity = (BYTE)parity;

    switch (parity)
    {
    case JM_SP_PAR_SPACE:
        if (self->databits == 8)
        {
            jm_log_write(JM_LOG_WARN, "Serial Port", 
                "Space parity with 8 data bits is not supported by POSIX systems.");
        }
        self->comm_config.dcb.fParity = TRUE;
        break;
    case JM_SP_PAR_MARK:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "Mark parity is not supported by POSIX systems.");
        self->comm_config.dcb.fParity = TRUE;
        break;
    case JM_SP_PAR_NONE:
        self->comm_config.dcb.fParity = FALSE;
        break;
    case JM_SP_PAR_ODD:
    case JM_SP_PAR_EVEN:
        self->comm_config.dcb.fParity = TRUE;
        break;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (!SetCommConfig(self->handle, &(self->comm_config), 
        sizeof(COMMCONFIG)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_set_stopbits(JMSerialPort *self, gint32 stopbits)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!(stopbits == JM_SP_SB_ONE || 
        stopbits == JM_SP_SB_ONE_POINT_FIVE || 
        stopbits == JM_SP_SB_TWO))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (self->stopbits != stopbits)
    {
        if (self->databits == 5 && stopbits == JM_SP_SB_TWO)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }

        if (stopbits == JM_SP_SB_ONE_POINT_FIVE && self->databits != 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->stopbits = stopbits;
    }

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    switch (stopbits)
    {
    case JM_SP_SB_ONE:
        self->comm_config.dcb.StopBits = ONESTOPBIT;
        break;
    case JM_SP_SB_ONE_POINT_FIVE:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "1.5 stop bit operation is not supported by POSIX.");
        if (self->databits != 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->comm_config.dcb.StopBits = ONE5STOPBITS;
        break;
    case JM_SP_SB_TWO:
        if (self->databits == 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        self->comm_config.dcb.StopBits = TWOSTOPBITS;
        break;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (!SetCommConfig(self->handle, &(self->comm_config), 
        sizeof(COMMCONFIG)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_GENERIC;
}

gint32 jm_serial_port_set_flow_control(JMSerialPort *self, 
    gint32 flow_control)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!(flow_control == JM_SP_FC_NONE || 
        flow_control == JM_SP_FC_HARDWARE || 
        flow_control == JM_SP_FC_SOFTWARE))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (self->flow_control != flow_control)
    {
        self->flow_control = flow_control;
    }

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    switch (flow_control)
    {
    case JM_SP_FC_NONE:
        self->comm_config.dcb.fOutxCtsFlow = FALSE;
        self->comm_config.dcb.fRtsControl = RTS_CONTROL_DISABLE;
        self->comm_config.dcb.fInX = FALSE;
        self->comm_config.dcb.fOutX = FALSE;
        break;
    case JM_SP_FC_SOFTWARE:
        self->comm_config.dcb.fOutxCtsFlow = FALSE;
        self->comm_config.dcb.fRtsControl = RTS_CONTROL_DISABLE;
        self->comm_config.dcb.fInX = TRUE;
        self->comm_config.dcb.fOutX = TRUE;
        break;
    case JM_SP_FC_HARDWARE:
        self->comm_config.dcb.fOutxCtsFlow = TRUE;
        self->comm_config.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        self->comm_config.dcb.fInX = FALSE;
        self->comm_config.dcb.fOutX = FALSE;
        break;
    default:
        break;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (!SetCommConfig(self->handle, &(self->comm_config), 
        sizeof(COMMCONFIG)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_set_write_timeout(JMSerialPort *self, gint64 millic)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    self->write_timeout = millic;

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (millic == -1)
    {
        self->comm_timeouts.WriteTotalTimeoutConstant = MAXDWORD;
        self->comm_timeouts.WriteTotalTimeoutMultiplier = MAXDWORD;
    }
    else
    {
        self->comm_timeouts.WriteTotalTimeoutConstant = (DWORD)millic;
        self->comm_timeouts.WriteTotalTimeoutMultiplier = 0;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (!SetCommTimeouts(self->handle, &(self->comm_timeouts)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_set_read_timeout(JMSerialPort *self, gint64 millic)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    self->read_timeout = millic;

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (millic == -1)
    {
        self->comm_timeouts.ReadIntervalTimeout = MAXDWORD;
        self->comm_timeouts.ReadTotalTimeoutConstant = 0;
    }
    else
    {
        self->comm_timeouts.ReadIntervalTimeout = (DWORD)millic;
        self->comm_timeouts.ReadTotalTimeoutConstant = (DWORD)millic;
    }

    self->comm_timeouts.ReadTotalTimeoutMultiplier = 0;

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    if (!SetCommTimeouts(self->handle, &(self->comm_timeouts)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gboolean jm_serial_port_is_open(JMSerialPort *self)
{
    gboolean ret;

    g_return_val_if_fail(self != NULL, FALSE);

    g_static_rec_mutex_lock(self->mutex);

    ret = self->handle != INVALID_HANDLE_VALUE;

    g_static_rec_mutex_unlock(self->mutex);

    return ret;
}

gint32 jm_serial_port_open(JMSerialPort *self)
{
    DWORD conf_size = sizeof(COMMCONFIG);
    DWORD dw_flags_and_attribute = FILE_FLAG_OVERLAPPED;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    self->comm_config.dwSize = conf_size;

    if (jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    /* Open the port */
    self->handle = CreateFileA(self->port_name, 
        GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 
        dw_flags_and_attribute, NULL);

    if (self->handle == INVALID_HANDLE_VALUE)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    /* Configure port setting */
    if (!GetCommConfig(self->handle, &(self->comm_config), &conf_size) || 
        !GetCommState(self->handle, &(self->comm_config.dcb)))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    /* Set up parameters */
    self->comm_config.dcb.fBinary = TRUE;
    self->comm_config.dcb.fInX = FALSE;
    self->comm_config.dcb.fOutX = FALSE;
    self->comm_config.dcb.fAbortOnError = FALSE;
    self->comm_config.dcb.fNull = FALSE;
    self->is_multi_setting = TRUE;
    if (jm_serial_port_set_baudrate(self, self->baudrate)
        || jm_serial_port_set_databits(self, self->databits)
        || jm_serial_port_set_parity(self, self->parity)
        || jm_serial_port_set_flow_control(self, self->flow_control)
        || jm_serial_port_set_read_timeout(self, self->read_timeout)
        || jm_serial_port_set_write_timeout(self, self->write_timeout))
    {
        self->is_multi_setting = FALSE;
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (!SetCommConfig(self->handle, &(self->comm_config), 
        sizeof(COMMCONFIG)))
    {
        self->is_multi_setting = FALSE;
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    if (!SetCommTimeouts(self->handle, &(self->comm_timeouts)))
    {
        self->is_multi_setting = FALSE;
        g_static_rec_mutex_unlock(self->mutex);
        return GetLastError();
    }

    self->is_multi_setting = FALSE;

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_close(JMSerialPort *self)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    self->bytes_to_write = 0;

    /* Force a flush */
    jm_serial_port_flush(self);
    CancelIo(self->handle);
    if (CloseHandle(self->handle))
    {
        self->handle = INVALID_HANDLE_VALUE;
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }
    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_GENERIC;
}

gint32 jm_serial_port_flush(JMSerialPort *self)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (!FlushFileBuffers(self->handle))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_discard_in_buffer(JMSerialPort *self)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (!PurgeComm(self->handle, PURGE_RXABORT | PURGE_RXCLEAR))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_discard_out_buffer(JMSerialPort *self)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (!PurgeComm(self->handle, PURGE_TXABORT | PURGE_TXCLEAR))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

size_t jm_serial_port_bytes_available(JMSerialPort *self)
{
    DWORD errors;
    COMSTAT status;

    g_return_val_if_fail(self != NULL, 0);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    if (!ClearCommError(self->handle, &errors, &status))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return (size_t)status.cbInQue;
}

gint32 jm_serial_port_set_dtr(JMSerialPort *self, gboolean set)
{
    DWORD dw_func;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (set)
    {
        dw_func = SETDTR;
    }
    else
    {
        dw_func = CLRDTR;
    }

    if (!EscapeCommFunction(self->handle, dw_func))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_set_rts(JMSerialPort *self, gboolean set)
{
    DWORD dw_func;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (set)
    {
        dw_func = SETRTS;
    }
    else
    {
        dw_func = CLRRTS;
    }

    if (!EscapeCommFunction(self->handle, dw_func))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

size_t jm_serial_port_read(JMSerialPort *self, guint8 *data, size_t count)
{
    DWORD ret_val = -1;

    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(data != NULL, 0);
    g_return_val_if_fail(count > 0, 0);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    if (!ReadFile(self->handle, (void*)data, (DWORD)count, &ret_val, 
        &(self->read_overlap)))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return 0;
        }
        GetOverlappedResult(self->handle, &(self->read_overlap), &ret_val, 
            TRUE);
    }

    jm_log_write_hex(JM_LOG_DEBUG, "Serial Port: read", data, ret_val);

    g_static_rec_mutex_unlock(self->mutex);
    return (size_t)ret_val;
}

size_t jm_serial_port_write(JMSerialPort *self, const guint8 *data, 
    size_t count)
{
    DWORD ret_val = -1;

    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(data != NULL, 0);
    g_return_val_if_fail(count > 0, 0);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    jm_log_write_hex(JM_LOG_DEBUG, "Serial Port: write", data, count);

    if (!WriteFile(self->handle, (const void*)data, (DWORD)count, &ret_val, 
        &(self->write_overlap)))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return 0;
        }
        WaitForSingleObject(self->write_overlap.hEvent, INFINITE);
        GetOverlappedResult(self->handle, &(self->write_overlap), &ret_val, 
            TRUE);
    }

    g_static_rec_mutex_unlock(self->mutex);
    return (size_t)ret_val;
}

JMStringArray* jm_serial_port_get_system_ports(void)
{
    return _jm_serial_port_enumerate_device_win(&GUID_DEVCLASS_PORTS);
}

#endif