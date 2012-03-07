#include "jmserialportprivate.h"

#ifdef G_OS_WIN32
gint32 _jm_serial_port_full_name_win(const gchar *name, gchar **result)
{
    GString *temp = g_string_new("");
    if (*result != NULL)
    {
        g_free(*result);
        *result = NULL;
    }
    if (name[0] != '\\')
        g_string_append(temp, "\\\\.\\");

    g_string_append(temp, name);
    if (temp->str[4] != 'C' || temp->str[5] != 'O' || temp->str[6] != 'M')
    {
        g_string_free(temp, TRUE);
        return JM_ERROR_GENERIC;
    }
    *result = g_string_free(temp, FALSE);
    return JM_ERROR_SUCCESS;
}

JMStringArray* _jm_serial_port_enumerate_device_win(const GUID* guid)
{
    JMStringArray *arr = jm_string_array_new();
    HDEVINFO dev_info;
    if ((dev_info = SetupDiGetClassDevsA(guid, NULL, NULL, DIGCF_PRESENT)) 
        != INVALID_HANDLE_VALUE)
    {
        SP_DEVINFO_DATA dev_info_data;
        DWORD i;
        dev_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
        for (i = 0; SetupDiEnumDeviceInfo(dev_info, i, &dev_info_data); 
            ++i)
        {
            HKEY dev_key = SetupDiOpenDevRegKey(dev_info, &dev_info_data, 
                DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            gchar* name = _jm_serial_port_get_reg_key_value(dev_key, 
                "PortName");
            gchar* full_name = NULL;
            gint32 ret = _jm_serial_port_full_name_win(name, &full_name);
            if (ret)
            {
                g_free(name);
                g_free(full_name);
                continue;
            }
            jm_string_array_append(arr, full_name);
            g_free(name);
            g_free(full_name);
        }
    }
    return arr;
}

gchar* _jm_serial_port_get_reg_key_value(HKEY key, const gchar *property)
{
    DWORD size = 0;
    DWORD type;
    BYTE *buff = NULL;
    RegQueryValueExA(key, property, NULL, NULL, NULL, &size);
    buff = (BYTE*)g_malloc(size);
    if (RegQueryValueExA(key, property, NULL, &type, buff, &size) 
        == ERROR_SUCCESS)
    {
        return (gchar*)buff;
    }
    g_free(buff);
    return NULL;
}

#else

gint32 _jm_serial_port_set_baudrate(JMSerialPort *priv, tcflag_t baud)
{
#ifdef CBAUD
    priv->comm_config.c_cflag &= (~CBAUD);
    priv->comm_config.c_cflag |= baud;
#else
    cfsetispeed(&priv->comm_config, baud);
    cfsetospeed(&priv->comm_config, baud);
#endif
    if (priv->is_multi_setting)
        return JM_ERROR_SUCCESS;
    return tcsetattr(priv->fd, TCSAFLUSH, &priv->comm_config);
}

gint32 _jm_serial_port_set_databits(JMSerialPort *priv, tcflag bits)
{
    priv->comm_config.c_cflag &= (~CSIZE);
    priv->comm_config.c_cflag |= bits;
    if (priv->is_multi_setting)
        return JM_ERROR_SUCCESS;
    return tcsetattr(priv->fd, TCSAFLUSH, &priv->comm_config);
}

#endif