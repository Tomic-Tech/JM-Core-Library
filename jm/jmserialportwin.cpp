#include "jmserialport.hpp"
#include "jmserialport.h"

#ifdef BOOST_WINDOWS

namespace JM
{
	// see http://msdn.microsoft.com/en-us/library/ms791134.aspx for list of GUID classes
#ifndef GUID_DEVCLASS_PORTS
	DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
#endif

	void SerialPort::platformInit()
	{
		_bytesToWrite = 0;
		_handle = INVALID_HANDLE_VALUE;
		ZeroMemory(&_commConfig, sizeof(COMMCONFIG));
		ZeroMemory(&_commTimeouts, sizeof(COMMTIMEOUTS));
		setPortName("\\\\.\\COM1");
	}

	void SerialPort::platformDestroy()
	{
	}

	boost::int32_t SerialPort::setPortName(const std::string &name)
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		boost::int32_t ret;
		if (isOpen())
		{
			return JM_ERROR_GENERIC;
		}

		ret = fullNameWin(name, _portName);
		return ret;
	}

	boost::int32_t SerialPort::setBaudrate(boost::int32_t baudrate)
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (_baudrate != baudrate)
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
				return JM_ERROR_GENERIC;
			default:
				_baudrate = baudrate;
				break;
			}
		}

		if (!isOpen())
		{
			return JM_ERROR_SUCCESS;
		}

		switch (baudrate)
		{
		case 110:
			_commConfig.dcb.BaudRate = CBR_110;
			break;
		case 300:
			_commConfig.dcb.BaudRate = CBR_300;
			break;
		case 600:
			_commConfig.dcb.BaudRate = CBR_600;
			break;
		case 1200:
			_commConfig.dcb.BaudRate = CBR_1200;
			break;
		case 2400:
			_commConfig.dcb.BaudRate = CBR_2400;
			break;
		case 4800:
			_commConfig.dcb.BaudRate = CBR_4800;
			break;
		case 9600:
			_commConfig.dcb.BaudRate = CBR_9600;
			break;
		case 14400:
			_commConfig.dcb.BaudRate = CBR_14400;
			break;
		case 19200:
			_commConfig.dcb.BaudRate = CBR_19200;
			break;
		case 38400:
			_commConfig.dcb.BaudRate = CBR_38400;
			break;
		case 57600:
			JM::Log::inst().write("Serial Port", "Set baud rate 57600");
			_commConfig.dcb.BaudRate = CBR_57600;
			break;
		case 115200:
			_commConfig.dcb.BaudRate = CBR_115200;
			break;
		case 128000:
			_commConfig.dcb.BaudRate = CBR_128000;
			break;
		case 256000:
			_commConfig.dcb.BaudRate = CBR_256000;
			break;
		default:
			
			return JM_ERROR_GENERIC;
		}

		if (_isMultiSetting)
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{	
			return GetLastError();
		}

		return JM_ERROR_SUCCESS;
	}

	boost::int32_t SerialPort::setDatabits(boost::uint8_t databits)
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (databits < 5 || databits > 8)
		{
			return JM_ERROR_GENERIC;
		}

		if (_databits != databits)
		{
			if (_stopbits == JM_SP_SB_TWO && databits == 5)
			{
				
				return JM_ERROR_GENERIC;
			}
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE && databits != 5)
			{
				
				return JM_ERROR_GENERIC;
			}
			if (_parity == JM_SP_PAR_SPACE && databits == 8)
			{
				
				return JM_ERROR_GENERIC;
			}
			_databits = databits;
		}
		

		if (!isOpen())
		{
			
			return JM_ERROR_SUCCESS;
		}

		switch (databits)
		{
		case 5:
			if (_stopbits == JM_SP_SB_TWO)
			{
				
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.ByteSize = 5;
			break;
		case 6:
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE)
			{
				
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.BaudRate = 6;
			break;
		case 7:
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE)
			{
				
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.ByteSize = 7;
			break;
		case 8:
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE)
			{
				
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.ByteSize = 8;
			break;
		}

		if (_isMultiSetting)
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			
			return GetLastError();
		}

		
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t SerialPort::setParity(boost::int32_t parity)
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!(parity == JM_SP_PAR_NONE || 
			parity == JM_SP_PAR_SPACE || 
			parity == JM_SP_PAR_EVEN || 
			parity == JM_SP_PAR_ODD || 
			parity == JM_SP_PAR_MARK))
		{
			
			return JM_ERROR_GENERIC;
		}

		if (_parity != parity)
		{
			_parity = parity;
		}

		if (!isOpen())
		{
			
			return JM_ERROR_SUCCESS;
		}

		_commConfig.dcb.Parity = (BYTE)parity;

		switch (parity)
		{
		case JM_SP_PAR_SPACE:
			if (_databits == 8)
			{
				JM::Log::inst().write("Serial Port", "Space parity with 8 data bits is not supported by POSIX systems.");
			}
			_commConfig.dcb.fParity = TRUE;
			break;
		case JM_SP_PAR_MARK:
			JM::Log::inst().write("Serial Port", "Mark parity is not supported by POSIX systems.");
			_commConfig.dcb.fParity = TRUE;
			break;
		case JM_SP_PAR_NONE:
			_commConfig.dcb.fParity = FALSE;
			break;
		case JM_SP_PAR_ODD:
		case JM_SP_PAR_EVEN:
			_commConfig.dcb.fParity = TRUE;
			break;
		}

		if (_isMultiSetting)
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			
			return JM_ERROR_GENERIC;
		}

		
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t SerialPort::setStopbits(boost::int32_t stopbits)
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!(stopbits == JM_SP_SB_ONE || 
			stopbits == JM_SP_SB_ONE_POINT_FIVE || 
			stopbits == JM_SP_SB_TWO))
		{
			
			return JM_ERROR_GENERIC;
		}

		if (_stopbits != stopbits)
		{
			if (_databits == 5 && stopbits == JM_SP_SB_TWO)
			{
				
				return JM_ERROR_GENERIC;
			}

			if (stopbits == JM_SP_SB_ONE_POINT_FIVE && _databits != 5)
			{
				
				return JM_ERROR_GENERIC;
			}
			_stopbits = stopbits;
		}

		if (!isOpen())
		{
			
			return JM_ERROR_SUCCESS;
		}

		switch (stopbits)
		{
		case JM_SP_SB_ONE:
			_commConfig.dcb.StopBits = ONESTOPBIT;
			break;
		case JM_SP_SB_ONE_POINT_FIVE:
			JM::Log::inst().write("Serial Port", "1.5 stop bit operation is not supported by POSIX.");
			if (_databits != 5)
			{
				
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.StopBits = ONE5STOPBITS;
			break;
		case JM_SP_SB_TWO:
			if (_databits == 5)
			{
				
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.StopBits = TWOSTOPBITS;
			break;
		}

		if (_isMultiSetting)
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			
			return JM_ERROR_GENERIC;
		}

		
		return JM_ERROR_GENERIC;
	}

	boost::int32_t SerialPort::setFlowControl(boost::int32_t flowControl)
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!(flowControl == JM_SP_FC_NONE || 
			flowControl == JM_SP_FC_HARDWARE || 
			flowControl == JM_SP_FC_SOFTWARE))
		{
			
			return JM_ERROR_GENERIC;
		}

		if (_flowControl != flowControl)
		{
			_flowControl = flowControl;
		}

		if (!isOpen())
		{
			
			return JM_ERROR_SUCCESS;
		}

		switch (flowControl)
		{
		case JM_SP_FC_NONE:
			_commConfig.dcb.fOutxCtsFlow = FALSE;
			_commConfig.dcb.fRtsControl = RTS_CONTROL_DISABLE;
			_commConfig.dcb.fInX = FALSE;
			_commConfig.dcb.fOutX = FALSE;
			break;
		case JM_SP_FC_SOFTWARE:
			_commConfig.dcb.fOutxCtsFlow = FALSE;
			_commConfig.dcb.fRtsControl = RTS_CONTROL_DISABLE;
			_commConfig.dcb.fInX = TRUE;
			_commConfig.dcb.fOutX = TRUE;
			break;
		case JM_SP_FC_HARDWARE:
			_commConfig.dcb.fOutxCtsFlow = TRUE;
			_commConfig.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			_commConfig.dcb.fInX = FALSE;
			_commConfig.dcb.fOutX = FALSE;
			break;
		default:
			break;
		}

		if (_isMultiSetting)
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			
			return GetLastError();
		}

		
		return JM_ERROR_SUCCESS;
	}

	bool SerialPort::isOpen()
	{
		return _handle != INVALID_HANDLE_VALUE;
	}

	boost::int32_t SerialPort::open()
	{
		DWORD confSize = sizeof(COMMCONFIG);
		DWORD dwFlagsAndAttribute = FILE_ATTRIBUTE_NORMAL;

		boost::recursive_mutex::scoped_lock lock(_mutex);

		_commConfig.dwSize = confSize;

		if (isOpen())
		{
			
			return JM_ERROR_GENERIC;
		}

		/* Open the port */
		_handle = CreateFileA(_portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, dwFlagsAndAttribute, NULL);

		if (_handle == INVALID_HANDLE_VALUE)
		{
			
			return GetLastError();
		}

		SetCommMask(_handle, EV_RXCHAR);
		SetupComm(_handle, 1024, 1024);

		/* Configure port setting */
		if (!GetCommConfig(_handle, &(_commConfig), &confSize) || 
			!GetCommState(_handle, &(_commConfig.dcb)))
		{
			
			return GetLastError();
		}

		/* Set up parameters */
		_commConfig.dcb.fInX = FALSE;
		_commConfig.dcb.fOutX = FALSE;
		_commConfig.dcb.fAbortOnError = FALSE;

		_isMultiSetting = TRUE;
		if (setBaudrate(_baudrate) ||
			setDatabits(_databits) ||
			setParity(_parity) ||
			setFlowControl(_flowControl) ||
			setReadTimeout(_readTimeout) ||
			setWriteTimeout(_writeTimeout))
		{
			_isMultiSetting = false;
			
			return JM_ERROR_GENERIC;
		}

		if (!SetCommConfig(_handle, &(_commConfig), sizeof(COMMCONFIG)))
		{
			_isMultiSetting = false;
			
			return GetLastError();
		}

		if (!SetCommTimeouts(_handle, &(_commTimeouts)))
		{
			_isMultiSetting = false;
			
			return GetLastError();
		}

		_isMultiSetting = false;

		
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t SerialPort::close()
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!isOpen())
		{
			
			return JM_ERROR_SUCCESS;
		}

		_bytesToWrite = 0;

		/* Force a flush */
		flush();
		CancelIo(_handle);
		if (CloseHandle(_handle))
		{
			_handle = INVALID_HANDLE_VALUE;
			
			return JM_ERROR_SUCCESS;
		}
		
		return JM_ERROR_GENERIC;
	}

	boost::int32_t SerialPort::flush()
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!isOpen())
		{
			
			return JM_ERROR_GENERIC;
		}

		if (!FlushFileBuffers(_handle))
		{
			
			return JM_ERROR_GENERIC;
		}

		
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t SerialPort::discardInBuffer()
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!isOpen())
		{
			
			return JM_ERROR_GENERIC;
		}

		if (!PurgeComm(_handle, PURGE_RXABORT | PURGE_RXCLEAR))
		{
			
			return JM_ERROR_GENERIC;
		}

		
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t SerialPort::discardOutBuffer()
	{
		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!isOpen())
		{
			
			return JM_ERROR_GENERIC;
		}

		if (!PurgeComm(_handle, PURGE_TXABORT | PURGE_TXCLEAR))
		{
			
			return JM_ERROR_GENERIC;
		}

		
		return JM_ERROR_SUCCESS;
	}

	size_t SerialPort::bytesAvailable()
	{
		DWORD errors;
		COMSTAT status;

		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!isOpen())
		{
			
			return 0;
		}

		if (!ClearCommError(_handle, &errors, &status))
		{
			
			return 0;
		}

		
		return (size_t)status.cbInQue;
	}

	boost::int32_t SerialPort::setDtr(bool set)
	{
		DWORD dw_func;

		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!isOpen())
		{
			
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

		if (!EscapeCommFunction(_handle, dw_func))
		{
			
			return JM_ERROR_GENERIC;
		}

		
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t SerialPort::setRts(bool set)
	{
		DWORD dw_func;

		boost::recursive_mutex::scoped_lock lock(_mutex);

		if (!isOpen())
		{
			
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

		if (!EscapeCommFunction(_handle, dw_func))
		{
			
			return JM_ERROR_GENERIC;
		}

		
		return JM_ERROR_SUCCESS;
	}

	std::vector<std::string> SerialPort::getSystemPorts()
	{
		return enumerateDeviceWin(&GUID_DEVCLASS_PORTS);
	}
}

#endif