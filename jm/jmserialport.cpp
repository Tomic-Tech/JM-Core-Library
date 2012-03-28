#include "jmserialport.h"
#include "jmserialport.hpp"
#include <boost/array.hpp>

namespace JM
{
	void SerialPort::init()
	{
		_portName = "";
		_readTimeout = boost::posix_time::milliseconds(500);
		_writeTimeout = boost::posix_time::milliseconds(500);
		_parity = JM_SP_PAR_NONE;
		_stopbits = JM_SP_SB_ONE;
		_flowControl = JM_SP_FC_NONE;
		_baudrate = 57600;
		_databits = 8;
		_isMultiSetting = false;
	}

	SerialPort::SerialPort()
	{
		init();
		platformInit();
	}

	SerialPort::SerialPort(const std::string &name)
	{
		init();
		platformInit();
		setPortName(name);
	}

	SerialPort::~SerialPort()
	{
		if (isOpen())
		{
			close();
		}

		platformDestroy();
	}

	std::string SerialPort::portName() const
	{
		return _portName;
	}

	boost::int32_t SerialPort::baudrate()
	{
		return _baudrate;
	}

	boost::uint8_t SerialPort::databits()
	{
		return _databits;
	}

	boost::int32_t SerialPort::stopbits()
	{
		return _stopbits;
	}

	boost::int32_t SerialPort::parity()
	{
		return _parity;
	}

	boost::int32_t SerialPort::flowControl()
	{
		return _flowControl;
	}

	boost::posix_time::time_duration SerialPort::readTimeout()
	{
		return _readTimeout;
	}

	boost::posix_time::time_duration SerialPort::writeTimeout()
	{
		return _writeTimeout;
	}


#ifdef BOOST_WINDOWS
	boost::int32_t SerialPort::fullNameWin(const std::string &name, std::string &result)
	{
		result.clear();
		if (name[0] != '\\')
			result.append("\\\\.\\");

		result.append(name);

		if (result[4] != 'C' || result[5] != 'O' || result[6] != 'M')
		{
			return JM_ERROR_GENERIC;
		}
		return JM_ERROR_SUCCESS;
	}

	std::vector<std::string> SerialPort::enumerateDeviceWin(const GUID* guid)
	{
		std::vector<std::string> arr;
		HDEVINFO dev_info;
		if ((dev_info = SetupDiGetClassDevsA(guid, NULL, NULL, DIGCF_PRESENT)) != INVALID_HANDLE_VALUE)
		{
			SP_DEVINFO_DATA dev_info_data;
			DWORD i;
			dev_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
			for (i = 0; SetupDiEnumDeviceInfo(dev_info, i, &dev_info_data); ++i)
			{
				HKEY dev_key = SetupDiOpenDevRegKey(dev_info, &dev_info_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
				std::string name = getRegKeyValue(dev_key, "PortName");
				std::string fullName;
				boost::int32_t ret = fullNameWin(name, fullName);
				if (ret)
				{
					continue;
				}
				arr.push_back(fullName);
			}
		}
		return arr;
	}

	std::string SerialPort::getRegKeyValue(HKEY key, const std::string &property)
	{
		DWORD size = 0;
		DWORD type;
		BYTE *buff = NULL;
		RegQueryValueExA(key, property.c_str(), NULL, NULL, NULL, &size);
		buff = (BYTE*)g_malloc(size);
		std::string ret;

		if (RegQueryValueExA(key, property.c_str(), NULL, &type, buff, &size) == ERROR_SUCCESS)
		{
			ret = std::string((char*)buff);
		}
		g_free(buff);
		return ret;
	}

#else

	boost::int32_t SerialPort::_setBaudrate(tcflag_t baud)
	{
#ifdef CBAUD
		_commConfig.c_cflag &= (~CBAUD);
		_commConfig.c_cflag |= baud;
#else
		cfsetispeed(&_commConfig, baud);
		cfsetospeed(&_commConfig, baud);
#endif
		if (_isMultiSetting)
			return JM_ERROR_SUCCESS;
		return tcsetattr(_fd, TCSAFLUSH, &_commConfig);
	}

	boost::int32_t _jm_serial_port_set_databits(tcflag bits)
	{
		_commConfig.c_cflag &= (~CSIZE);
		_commConfig.c_cflag |= bits;
		if (_isMultiSetting)
			return JM_ERROR_SUCCESS;
		return tcsetattr(_fd, TCSAFLUSH, &_commConfig);
	}

#endif
}

#include "jmserialport.h"
JMSerialPort* jm_serial_port_new(void)
{
	JMSerialPort *obj = (JMSerialPort*)g_malloc(sizeof(JMSerialPort));
	obj->_handle = (gpointer)new JM::SerialPort();
	return obj;
}

JMSerialPort* jm_serial_port_new_with_name(const gchar *name)
{
	JMSerialPort *obj = (JMSerialPort*)g_malloc(sizeof(JMSerialPort));
	obj->_handle = (gpointer)new JM::SerialPort(name);
	return obj;
}

void jm_serial_port_free(JMSerialPort *self)
{
	g_return_if_fail(self != NULL);

	delete ((JM::SerialPort*)(self->_handle));
	g_free(self);
}

const gchar* jm_serial_port_get_port_name(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, NULL);
	return ((JM::SerialPort*)(self->_handle))->portName().c_str();
}

gint32 jm_serial_port_set_port_name(JMSerialPort *self, 
	const gchar *name)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setPortName(name);
}

gint32 jm_serial_port_get_baudrate(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->baudrate();
}

gint32 jm_serial_port_set_baudrate(JMSerialPort *self, gint32 baudrate)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setBaudrate(baudrate);
}

guint8 jm_serial_port_get_databits(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->databits();
}

gint32 jm_serial_port_set_databits(JMSerialPort *self, guint8 databits)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setDatabits(databits);
}

gint32 jm_serial_port_get_stopbits(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->stopbits();
}

gint32 jm_serial_port_set_stopbits(JMSerialPort *self, gint32 stopbits)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setStopbits(stopbits);
}

gint32 jm_serial_port_get_parity(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->parity();
}

gint32 jm_serial_port_set_parity(JMSerialPort *self, gint32 parity)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setParity(parity);
}

gint32 jm_serial_port_get_flow_control(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->flowControl();
}

gint32 jm_serial_port_set_flow_control(JMSerialPort *self, 
	gint32 flow_control)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setFlowControl(flow_control);
}

size_t jm_serial_port_write(JMSerialPort *self, const guint8 *data, size_t count)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->write(boost::asio::const_buffer(data, count));
}

size_t jm_serial_port_read(JMSerialPort *self, guint8 *data, size_t count)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->read(boost::asio::mutable_buffer(data, count));
}

gint64 jm_serial_port_get_read_timeout(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->readTimeout().total_microseconds();
}

gint32 jm_serial_port_set_read_timeout(JMSerialPort *self, gint64 microseconds)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setReadTimeout(boost::posix_time::microseconds(microseconds));
}

gint64 jm_serial_port_get_write_timeout(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->writeTimeout().total_microseconds();
}

gint32 jm_serial_port_set_write_timeout(JMSerialPort *self, gint64 microseconds)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setWriteTimeout(boost::posix_time::microseconds(microseconds));
}

gboolean jm_serial_port_is_open(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, false);
	return ((JM::SerialPort*)(self->_handle))->isOpen();
}

gint32 jm_serial_port_open(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->open();
}

gint32 jm_serial_port_close(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->close();
}

gint32 jm_serial_port_flush(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->flush();
}

gint32 jm_serial_port_discard_in_buffer(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->discardInBuffer();
}

gint32 jm_serial_port_discard_out_buffer(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->discardOutBuffer();
}

size_t jm_serial_port_bytes_available(JMSerialPort *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return ((JM::SerialPort*)(self->_handle))->bytesAvailable();
}

gint32 jm_serial_port_set_dtr(JMSerialPort *self, gboolean set)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setDtr(set ? true : false);
}

gint32 jm_serial_port_set_rts(JMSerialPort *self, gboolean set)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
	return ((JM::SerialPort*)(self->_handle))->setRts(set ? true : false);
}

JMStringArray* jm_serial_port_get_system_ports(void)
{
	JMStringArray *arr = jm_string_array_new();
	std::vector<std::string> vec = JM::SerialPort::getSystemPorts();
	for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		jm_string_array_append(arr, it->c_str());
	}
	return arr;
}

