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
		boost::scoped_array<BYTE> buff;
		RegQueryValueExA(key, property.c_str(), NULL, NULL, NULL, &size);
		buff.reset(new BYTE[size]);
		std::string ret;

		if (RegQueryValueExA(key, property.c_str(), NULL, &type, buff.get(), &size) == ERROR_SUCCESS)
		{
			ret = std::string((char*)buff.get());
		}
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
	JMSerialPort *obj = new JMSerialPort;
	obj->_handle = (pointer_t)new JM::SerialPort;
	return obj;
}

JMSerialPort* jm_serial_port_new_with_name(const char *name)
{
	JMSerialPort *obj = new JMSerialPort;
	obj->_handle = (pointer_t)new JM::SerialPort(name);
	return obj;
}

void jm_serial_port_free(JMSerialPort *self)
{
	if (self == NULL)
		return;

	delete ((JM::SerialPort*)(self->_handle));
	delete self;
}

const char* jm_serial_port_get_port_name(JMSerialPort *self)
{
	if (self == NULL)
		return NULL;
	static boost::array<char, 255> temp;
	strcpy(temp.data(), ((JM::SerialPort*)(self->_handle))->portName().c_str());
	return temp.data();
}

int32_t jm_serial_port_set_port_name(JMSerialPort *self, const char *name)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->setPortName(name);
}

int32_t jm_serial_port_get_baudrate(JMSerialPort *self)
{
	if (self != NULL)
		return 0;
	return ((JM::SerialPort*)(self->_handle))->baudrate();
}

int32_t jm_serial_port_set_baudrate(JMSerialPort *self, int32_t baudrate)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->setBaudrate(baudrate);
}

uint8_t jm_serial_port_get_databits(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->databits();
}

int32_t jm_serial_port_set_databits(JMSerialPort *self, uint8_t databits)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->setDatabits(databits);
}

int32_t jm_serial_port_get_stopbits(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->stopbits();
}

int32_t jm_serial_port_set_stopbits(JMSerialPort *self, int32_t stopbits)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->setStopbits(stopbits);
}

int32_t jm_serial_port_get_parity(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->parity();
}

int32_t jm_serial_port_set_parity(JMSerialPort *self, int32_t parity)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->setParity(parity);
}

int32_t jm_serial_port_get_flow_control(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->flowControl();
}

int32_t jm_serial_port_set_flow_control(JMSerialPort *self, int32_t flow_control)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->setFlowControl(flow_control);
}

size_t jm_serial_port_write(JMSerialPort *self, const uint8_t *data, size_t count)
{
	if (self == NULL)
		return 0;

	return ((JM::SerialPort*)(self->_handle))->write(boost::asio::const_buffer(data, count));
}

size_t jm_serial_port_read(JMSerialPort *self, uint8_t *data, size_t count)
{
	if (self == NULL)
		return 0;

	return ((JM::SerialPort*)(self->_handle))->read(boost::asio::mutable_buffer(data, count));
}

int64_t jm_serial_port_get_read_timeout(JMSerialPort *self)
{
	if (self == NULL)
		return 0;

	return ((JM::SerialPort*)(self->_handle))->readTimeout().total_microseconds();
}

int32_t jm_serial_port_set_read_timeout(JMSerialPort *self, int64_t microseconds)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->setReadTimeout(boost::posix_time::microseconds(microseconds));
}

int64_t jm_serial_port_get_write_timeout(JMSerialPort *self)
{
	if (self == NULL)
		return 0;
	return ((JM::SerialPort*)(self->_handle))->writeTimeout().total_microseconds();
}

int32_t jm_serial_port_set_write_timeout(JMSerialPort *self, int64_t microseconds)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;
	return ((JM::SerialPort*)(self->_handle))->setWriteTimeout(boost::posix_time::microseconds(microseconds));
}

boolean_t jm_serial_port_is_open(JMSerialPort *self)
{
	if (self == NULL)
		return FALSE;
	return ((JM::SerialPort*)(self->_handle))->isOpen();
}

int32_t jm_serial_port_open(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->open();
}

int32_t jm_serial_port_close(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->close();
}

int32_t jm_serial_port_flush(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->flush();
}

int32_t jm_serial_port_discard_in_buffer(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->discardInBuffer();
}

int32_t jm_serial_port_discard_out_buffer(JMSerialPort *self)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->discardOutBuffer();
}

size_t jm_serial_port_bytes_available(JMSerialPort *self)
{
	if (self == NULL)
		return 0;

	return ((JM::SerialPort*)(self->_handle))->bytesAvailable();
}

int32_t jm_serial_port_set_dtr(JMSerialPort *self, boolean_t set)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

	return ((JM::SerialPort*)(self->_handle))->setDtr(set ? true : false);
}

int32_t jm_serial_port_set_rts(JMSerialPort *self, boolean_t set)
{
	if (self == NULL)
		return JM_ERROR_GENERIC;

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

