#include "jmserialportprivate.h"

#ifdef G_OS_LINUX

void _jm_serial_port_platform_destroy(JMSerialPort *self)
{
    self->fd = -1;
    memset(&(self->comm_config, 0, sizeof(struct termios)));
    memset(&self->old_termios, 0, sizeof (struct termios));
    memset(&self->timeout, 0, sizeof (struct timeval));
    memset(&self->copy_timeout, 0, sizeof (struct timeval));
    jm_serial_port_set_port_name(self, "/dev/ttyS0");
}

void _jm_serial_port_platform_destroy(JMSerialPort *self)
{

}

gint32 jm_serial_port_set_port_name(JMSerialPort *self, const gchar *name)
{
    gulong length;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (name == NULL)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (self->port_name != NULL)
        g_free(self->port_name);

    length = g_utf8_strlen(name);

    self->port_name = (gchar*)g_malloc(length + 1);

    g_utf8_strncpy(self->port_name, name, length);
    self->port_name[length] = 0;

    g_static_rec_mutex_unlock(self->mutex);
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_set_baudrate(JMSerialPort *self, gint32 baudrate)
{
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (self->baudrate != baudrate)
    {
        switch (baudrate)
        {
        case 14400:
        case 56000:
        case 128000:
        case 256000:
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        case 76800:
#ifndef B76800
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
#else
            self->baudrate = baudrate;
#endif
            break;
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
    case 50:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "Windows does not support 50 baud operation.");
        ret = _jm_serial_port_set_baudrate(self, B50);
        break;
    case 75:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "Windows does no support 75 baud operation.");
        ret = _jm_serial_port_set_baudrate(self, B75);
        break;
    case 110:
        ret = _jm_serial_port_set_baudrate(self, B110);
        break;
    case 134:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "Windows does no support 134.5 baud operation.");
        ret = _jm_serial_port_set_baudrate(self, B134);
        break;
    case 150:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "Windows does no support 150 baud operation.");
        ret = _jm_serial_port_set_baudrate(self, B150);
        break;
    case 200:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "Windows does no support 200 baud operation.");
        ret = _jm_serial_port_set_baudrate(self, B200);
        break;
    case 300:
        ret = _jm_serial_port_set_baudrate(self, B300);
        break;
    case 600:
        ret = _jm_serial_port_set_baudrate(self, B600);
        break;
    case 1200:
        ret = _jm_serial_port_set_baudrate(self, B1200);
        break;
    case 1800:
        ret = _jm_serial_port_set_baudrate(self, B1800);
        break;
    case 2400:
        ret = _jm_serial_port_set_baudrate(self, B2400);
        break;
    case 4800:
        ret = _jm_serial_port_set_baudrate(self, B4800);
        break;
    case 9600:
        ret = _jm_serial_port_set_baudrate(self, B9600);
        break;
    case 19200:
        ret = _jm_serial_port_set_baudrate(self, B19200);
        break;
    case 38400:
        ret = _jm_serial_port_set_baudrate(self, B38400);
        break;
    case 57600:
        ret = _jm_serial_port_set_baudrate(self, B57600);
        break;
    case 76800:
        jm_log_write(JM_LOG_WARN, "Serial Port", 
            "Windows and some POSIX systems do not support 76800 baud operation.");
#ifdef B76800
        ret = _jm_serial_port_set_baudrate(self, B76800);
#else
        ret = JM_ERROR_GENERIC;
#endif
        break;
    case 115200:
        ret = _jm_serial_port_set_baudrate(self, B115200);
        break;
    case 256000:
    case 128000:
    case 56000:
    case 14400:
    default:
        ret = JM_ERROR_GENERIC;
        break;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_set_databits(JMSerialPort *self, guint8 databits)
{
    gint32 ret = JM_ERROR_GENERIC;

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
        else if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE && databits != 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        else if (self->parity == JM_SP_PAR_SPACE && databits == 8)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        else
        {
            self->databits = databits;
        }
    }

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
            ret = JM_ERROR_GENERIC;
        }
        else
        {
            self->databits = databits;
            ret = _jm_serial_port_set_databits(self, CS5);
        }
        break;
    case 6:
        if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE)
        {
            ret = JM_ERROR_GENERIC;
        }
        else
        {
            self->databits = databits;
            ret = _jm_serial_port_set_databits(self, CS6);
        }
        break;
    case 7:
        if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE)
        {
            ret = JM_ERROR_GENERIC;
        }
        else
        {
            self->databits = databits;
            ret = _jm_serial_port_set_databits(self, CS7);
        }
        break;
    case 8:
        if (self->stopbits == JM_SP_SB_ONE_POINT_FIVE)
        {
            ret = JM_ERROR_GENERIC;
        }
        else
        {
            self->databits = databits;
            ret = _jm_serial_port_set_databits(self, CS8);
        }
        break;
    }

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_set_parity(JMSerialPort *self, gint32 parity)
{
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!(parity == JM_SP_PAR_EVEN || 
        parity == JM_SP_PAR_MARK || 
        parity == JM_SP_PAR_NONE || 
        parity == JM_SP_PAR_ODD || 
        parity == JM_SP_PAR_SPACE))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    if (self->parity != parity)
    {
        if (parity == JM_SP_PAR_MARK || 
            (parity == JM_SP_PAR_SPACE && self->databits == 8))
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        else
        {
            self->parity = parity;
        }
    }

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    switch (parity)
    {
    case JM_SP_PAR_SPACE:
        if (self->databits == 8)
        {
            ret = JM_ERROR_GENERIC;
        }
        else
        {
            /* space parity not directly supported - add an extra data bit to simulate it */
            self->comm_config.c_cflag &= ~(PARENB | CSIZE);
            switch (self->databits)
            {
            case 5:
                self->databits = 6;
                self->comm_config.c_cflag |= CS6;
                break;
            case 6:
                self->databits = 7;
                self->comm_config.c_cflag |= CS7;
                break;
            case 7:
                self->databits = 8;
                self->comm_config.c_cflag |= CS8;
                break;
            case 8:
                break;
            }
        }
        break;

    case JM_SP_PAR_MARK:
        ret = JM_ERROR_GENERIC;
        break;
    case JM_SP_PAR_NONE:
        self->comm_config.c_cflag &= (~PARENB);
        break;
    case JM_SP_PAR_EVEN:
        self->comm_config.c_cflag &= (~PARODD);
        self->comm_config.c_cflag |= PARENB;
        break;
    case JM_SP_PAR_ODD:
        self->comm_config.c_cflag |= (PARENB | PARODD);
        break;
    }
    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    ret = tcsetattr(self->fd, TCSAFLUSH, &self->comm_config);
    g_static_rec_mutex_unlock(self->mutex);

    return ret;
}

gint32 jm_serial_port_set_stopbits(JMSerialPort *self, gint32 stopbits)
{
    gint32 ret = JM_ERROR_GENERIC;

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
        else if (stopbits == JM_SP_SB_ONE_POINT_FIVE && self->databits != 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC
        }
        else
        {
            self->stopbits = stopbits;
        }
    }

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    switch (stopbits)
    {
    case JM_SP_SB_ONE:
        self->stopbits = stopbits;
        self->comm_config.c_cflag &= (~CSTOPB);
        break;
    case JM_SP_SB_ONE_POINT_FIVE:
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
        break;
    case JM_SP_SB_TWO:
        if (self->databits == 5)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return JM_ERROR_GENERIC;
        }
        else
        {
            self->stopbits = stopbits;
            self->comm_config.c_cflag |= CSTOPB;
        }
        break;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    ret = tcsetattr(self->fd, TCSAFLUSH, &self->comm_config);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_set_flow_control(JMSerialPort *self, 
    gint32 flow_control)
{
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!(flow_control == JM_SP_FC_HARDWARE || 
        flow_control == JM_SP_FC_SOFTWARE || 
        flow_control == JM_SP_FC_NONE))
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
        self->comm_config.c_cflag &= (~CRTSCTS);
        self->comm_config.c_iflag &= (~(IXON | IXOFF | IXANY));
        break;

    case JM_SP_FC_SOFTWARE:
        self->comm_config.c_cflag &= (~CRTSCTS);
        self->comm_config.c_iflag |= (IXON | IXOFF | IXANY);
        break;

    case JM_SP_FC_HARDWARE:
        self->comm_config.c_cflag |= CRTSCTS;
        self->comm_config.c_iflag &= (~(IXON | IXOFF | IXANY));
        break;
    }

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    ret = tcsetattr(self->fd, TCSAFLUSH, &self->comm_config);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_set_read_timeout(JMSerialPort *self, gint64 millic)
{
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_mutex_lock(self->mutex);

    self->read_timeout = millic;

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    self->copy_timeout.tv_sec = millic / 1000;
    self->copy_timeout.tv_usec = millic % 1000;

    if (millic == -1)
    {
        ret = fcntl(self->fd, F_SETFL, O_NDELAY);
        if (ret)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return ret;
        }
    }
    else
    {
        /* O_SYNC should enable blocking write()
        * however this seems not working on Linux 2.6.21 (works on
        * OpenBSD 4.2)
        */
        ret = fcntl(self->fd, F_SETFL, O_NDELAY);
        if (ret)
        {
            g_static_rec_mutex_unlock(self->mutex);
            return ret;
        }
    }

    ret = tcgetattr(self->fd, &self->comm_config);
    if (error_code)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return ret;
    }

    self->comm_config.c_cc[VTIME] = millic / 100;

    if (self->is_multi_setting)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_SUCCESS;
    }

    ret = tcsetattr(self->fd, TCSAFLUSH, &self->comm_config);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_set_write_timeout(JMSerialPort *self, gint64 millic)
{
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    self->write_timeout = millic;

    g_static_rec_mutex_unlock(self->mutex);

    return JM_ERROR_SUCCESS;
}

gboolean jm_serial_port_is_open(JMSerialPort *self)
{
    gboolean ret = FALSE;

    g_return_val_if_fail(self != NULL, FALSE);

    g_static_rec_mutex_lock(self->mutex);

    ret = self->fd != -1;

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_open(JMSerialPort *self)
{
    const long vdisable;
    GString *debug = NULL;
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    vdisable = fpathconf(self->fd, _PC_VDISABLE);

    g_static_rec_mutex_lock(self->mutex);

    if (jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    debug = g_string_new("trying to open file ");
    g_string_append_printf(debug, "%s", self->port_name);
    jm_log_write(JM_LOG_DEBUG, "Serial Port", debug->str);
    g_string_free(debug, TRUE);

    /* note: linux 2.6.21 seems to ignore O_NDELAY flag */
    self->fd = open(self->port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (self->fd == -1)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    jm_log_write(JM_LOG_DEBUG, "Serial Port", "File opened successfully");

    /* Save the old termios */
    ret = tcgetattr(self->fd, &self->old_termios);
    if (ret != 0)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return error_code;
    }

    self->comm_config = self->old_termios; /* Make a working copy */
    cfmakeraw(&self->comm_config); /* Enable raw access */

    /* set up other port settings */
    self->comm_config.c_cflag |= CREAD | CLOCAL;
    self->comm_config.c_lflag &= 
        (~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG));
    self->comm_config.c_iflag &= 
        (~(INPCK | IGNPAR | PARMRK | ISTRIP | ICRNL | IXANY));
    self->comm_config.c_oflag &= (~OPOST);
    self->comm_config.c_cc[VMIN] = 0;
#ifdef _POSIX_VDISABLE /* Is a disable character available on this system ? */
    /* Some systems allow for per-device disable-characters, so get the
    * proper value for the configured device.
    */
    self->comm_config.c_cc[VINTR] = vdisable;
    self->comm_config.c_cc[VQUIT] = vdisable;
    self->comm_config.c_cc[VSTART] = vdisable;
    self->comm_config.c_cc[VSTOP] = vdisable;
    self->comm_config.c_cc[VSUSP] = vdisable;
#endif /* _POSIX_VDISABLE */
    self->is_multi_setting = TRUE;
    if (jm_serial_port_set_baudrate(self->baudrate)
        || jm_serial_port_set_databits(self->databits)
        || jm_serial_port_set_parity(self->parity)
        || jm_serial_port_set_stopbits(self->stopbits)
        || jm_serial_port_set_flow_control(self->flow_control)
        || jm_serial_port_set_read_timeout(self->read_timeout)
        || jm_serial_port_set_write_timeout(self->write_timeout))
    {
        g_static_rec_mutex_unlock(self->mutex);
        self->is_multi_setting = FALSE;
        return JM_ERROR_GENERIC;
    }

    ret = tcsetattr(self->fd, TCSAFLUSH, &self->comm_config);
    if (ret)
        close();
    self->is_multi_setting = FALSE;

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_close(JMSerialPort *self)
{
    const long vdisable;
    gint32 ret = JM_ERROR_GENERIC;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self);

    jm_log_write(JM_LOG_DEBUG, "Serial Port", "Close");

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    vdisable = fpathconf(self->fd, _PC_VDISABLE);

    /* Force a flush */
    jm_serial_port_flush(self);

    /* Using both TCSAFLUSH and TCSANOW here discards any pending input */
    ret = tcsetattr(self->fd, 
        TCSAFLUSH | TCSANOW, &(self->old_termios)); /* Restore termios */
    if (ret != 0)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return ret;
    }

    ret = close(self->fd);

    if (ret != 0)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return ret;
    }

    self->fd = -1;
    return JM_ERROR_SUCCESS;
}

gint32 jm_serial_port_flush(JMSerialPort *self)
{
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    ret = tcflush(self->fd, TCIOFLUSH);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_discard_in_buffer(JMSerialPort *self)
{
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self != NULL, JM_ERROR_GENERIC);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    ret = tcflush(self->fd, TCIFLUSH);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_discard_out_buffer(JMSerialPort *self)
{
    gint32 ret = JM_ERROR_GENERIC;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    ret = tcflush(self->fd, TCOFLUSH);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

size_t jm_serial_port_bytes_available(JMSerialPort *self)
{
    gint32 ret = JM_ERROR_GENERIC;
    int bytes_queued;

    g_return_val_if_fail(self != NULL, 0);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    ret = ioctl(self->fd, FIONREAD, &bytes_queued);
    if (ret != 0)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    return (size_t)bytes_queued;
}

gint32 jm_serial_port_set_dtr(JMSerialPort *self, gboolean set)
{
    gint32 ret = JM_ERROR_GENERIC;
    int status;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    ret = ioctl(self->fd, TIOCMGET, &status);
    if (ret != 0)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return ret;
    }

    if (set)
    {
        status |= TIOCM_DTR;
    }
    else
    {
        status &= ~TIOCM_DTR;
    }

    ret = ioctl(self->fd, TIOCMSET, &status);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

gint32 jm_serial_port_set_rts(JMSerialPort *self, gboolean set)
{
    gint32 ret = JM_ERROR_GENERIC;
    int status;

    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    g_static_rec_mutex_lock(self->mutex);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return JM_ERROR_GENERIC;
    }

    ret = ioctl(self->fd, TIOCMGET, &status);
    if (ret != 0)
    {
        g_static_rec_mutex_unlock(self->mutex);
        return ret;
    }

    if (set)
    {
        status |= TIOCM_RTS;
    }
    else
    {
        status &= ~TIOCM_RTS;
    }

    ret = ioctl(self->fd, TIOCMSET, &status);

    g_static_rec_mutex_unlock(self->mutex);
    return ret;
}

size_t jm_serial_port_read(JMSerialPort *self, guint8 *data, size_t count)
{
    gint32 ret = 0;

    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(data != NULL, 0);
    g_return_val_if_fail(count > 0, 0);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    ret = read(self->fd, data, count);

    g_static_rec_mutex_unlock(self->mutex);
    return (size_t)ret;
}

size_t jm_serial_port_write(JMSerialPort *self, const guint8 *data, 
    size_t count)
{
    gint32 ret = 0;

    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(data != NULL, 0);
    g_return_val_if_fail(count > 0, 0);

    if (!jm_serial_port_is_open(self))
    {
        g_static_rec_mutex_unlock(self->mutex);
        return 0;
    }

    ret = write(self->fd, data, count);
    return (size_t)ret;
}

JMStringArray* jm_serial_port_get_system_ports(void)
{
    JMStringArray *arr = jm_string_array_new();
    struct dirent *ent = NULL;
    DIR *dir_p;
    char dir[512];
    struct stat statbuf;

    if ((dir_p = opendir("/dev")) == NULL)
    {
        return arr;
    }

    while ((ent = readdir(dir_p)) != NULL)
    {
        /* get file absolute name */
        snprintf(dir, 512, "%s/%s", "/dev", ent->d_name);
        /* get file information */
        lstat(dir, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            /* skip directory */
        }
        else
        {
            if ((ent->d_name[0] == 't') && (ent->d_name[1] == 't')
                && (ent->d_name[2] == 'y')) {
                    if ((ent->d_name[3] == 'S') || 
                        ((ent->d_name[3] == 'U') && 
                        (ent->d_name[4] == 'S') && 
                        (ent->d_name[5] == 'B')) ||
                        ((ent->d_name[3] == 'A') &&
                        (ent->d_name[4] == 'C') &&
                        (ent->d_name[5] == 'M')))
                    {
                        jm_string_array_append(arr, ent->d_name);
                    }
            }
            else if ((ent->d_name[0] == 'r') && (ent->d_name[1] == 'f')
                && (ent->d_name[2] == 'c') && (ent->d_name[3] == 'o')
                && (ent->d_name[4] == 'm') && (ent->d_name[5] == 'm'))
            {
                jm_string_array_append(arr, ent->d_name);
            }
        }
    }
    return arr;
}

#endif