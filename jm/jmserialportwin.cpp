#include "jmserialport.hpp"
#include "jmserialport.h"
#include "jmlib.h"

namespace JM
{
#ifdef G_OS_WIN32

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

	gint32 SerialPort::setPortName(const gchar *name)
	{
		gint32 ret;
		g_return_val_if_fail(name != NULL, JM_ERROR_GENERIC);

		g_mutex_lock(_mutex);
		if (isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		ret = fullNameWin(name, &_portName);
		g_mutex_unlock(_mutex);
		return ret;
	}

	gint32 SerialPort::setBaudrate(gint32 baudrate)
	{
		g_mutex_lock(_mutex);

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
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			default:
				_baudrate = baudrate;
				break;
			}
		}

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
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
			jm_log_write(JM_LOG_DEBUG, "Serial Port", "Set baud rate 57600");
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
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (_isMultiSetting)
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::setDatabits(guint8 databits)
	{
		g_mutex_lock(_mutex);

		if (databits < 5 || databits > 8)
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (_databits != databits)
		{
			if (_stopbits == JM_SP_SB_TWO && databits == 5)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE && databits != 5)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			if (_parity == JM_SP_PAR_SPACE && databits == 8)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_databits = databits;
		}
		g_mutex_unlock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		switch (databits)
		{
		case 5:
			if (_stopbits == JM_SP_SB_TWO)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.ByteSize = 5;
			break;
		case 6:
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.BaudRate = 6;
			break;
		case 7:
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.ByteSize = 7;
			break;
		case 8:
			if (_stopbits == JM_SP_SB_ONE_POINT_FIVE)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.ByteSize = 8;
			break;
		}

		if (_isMultiSetting)
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::setParity(gint32 parity)
	{
		g_mutex_lock(_mutex);

		if (!(parity == JM_SP_PAR_NONE || 
			parity == JM_SP_PAR_SPACE || 
			parity == JM_SP_PAR_EVEN || 
			parity == JM_SP_PAR_ODD || 
			parity == JM_SP_PAR_MARK))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (_parity != parity)
		{
			_parity = parity;
		}

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		_commConfig.dcb.Parity = (BYTE)parity;

		switch (parity)
		{
		case JM_SP_PAR_SPACE:
			if (_databits == 8)
			{
				jm_log_write(JM_LOG_WARN, "Serial Port", "Space parity with 8 data bits is not supported by POSIX systems.");
			}
			_commConfig.dcb.fParity = TRUE;
			break;
		case JM_SP_PAR_MARK:
			jm_log_write(JM_LOG_WARN, "Serial Port", "Mark parity is not supported by POSIX systems.");
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
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::setStopbits(gint32 stopbits)
	{
		g_mutex_lock(_mutex);

		if (!(stopbits == JM_SP_SB_ONE || 
			stopbits == JM_SP_SB_ONE_POINT_FIVE || 
			stopbits == JM_SP_SB_TWO))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (_stopbits != stopbits)
		{
			if (_databits == 5 && stopbits == JM_SP_SB_TWO)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}

			if (stopbits == JM_SP_SB_ONE_POINT_FIVE && _databits != 5)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_stopbits = stopbits;
		}

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		switch (stopbits)
		{
		case JM_SP_SB_ONE:
			_commConfig.dcb.StopBits = ONESTOPBIT;
			break;
		case JM_SP_SB_ONE_POINT_FIVE:
			jm_log_write(JM_LOG_WARN, "Serial Port", "1.5 stop bit operation is not supported by POSIX.");
			if (_databits != 5)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.StopBits = ONE5STOPBITS;
			break;
		case JM_SP_SB_TWO:
			if (_databits == 5)
			{
				g_mutex_unlock(_mutex);
				return JM_ERROR_GENERIC;
			}
			_commConfig.dcb.StopBits = TWOSTOPBITS;
			break;
		}

		if (_isMultiSetting)
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_GENERIC;
	}

	gint32 SerialPort::setFlowControl(gint32 flowControl)
	{
		g_mutex_lock(_mutex);

		if (!(flowControl == JM_SP_FC_NONE || 
			flowControl == JM_SP_FC_HARDWARE || 
			flowControl == JM_SP_FC_SOFTWARE))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (_flowControl != flowControl)
		{
			_flowControl = flowControl;
		}

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
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
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommState(_handle, &(_commConfig.dcb)))
		{
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::setWriteTimeout(gint64 millic)
	{
		g_mutex_lock(_mutex);

		_writeTimeout = millic;

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (millic == -1)
		{
			_commTimeouts.WriteTotalTimeoutConstant = MAXDWORD;
			_commTimeouts.WriteTotalTimeoutMultiplier = MAXDWORD;
		}
		else
		{
			_commTimeouts.WriteTotalTimeoutConstant = (DWORD)millic;
			_commTimeouts.WriteTotalTimeoutMultiplier = 0;
		}

		if (_isMultiSetting)
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommTimeouts(_handle, &(_commTimeouts)))
		{
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::setReadTimeout(gint64 millic)
	{
		g_mutex_lock(_mutex);

		_readTimeout = millic;

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (millic == -1)
		{
			_commTimeouts.ReadIntervalTimeout = MAXDWORD;
			_commTimeouts.ReadTotalTimeoutConstant = 0;
		}
		else
		{
			_commTimeouts.ReadIntervalTimeout = (DWORD)millic;
			_commTimeouts.ReadTotalTimeoutConstant = (DWORD)millic;
		}

		_commTimeouts.ReadTotalTimeoutMultiplier = 0;

		if (_isMultiSetting)
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommTimeouts(_handle, &(_commTimeouts)))
		{
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gboolean SerialPort::isOpen()
	{
		return _handle != INVALID_HANDLE_VALUE;
	}

	gint32 SerialPort::open()
	{
		DWORD confSize = sizeof(COMMCONFIG);
		DWORD dwFlagsAndAttribute = FILE_ATTRIBUTE_NORMAL;

		g_mutex_lock(_mutex);

		_commConfig.dwSize = confSize;

		if (isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		/* Open the port */
		_handle = CreateFileA(_portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, dwFlagsAndAttribute, NULL);

		if (_handle == INVALID_HANDLE_VALUE)
		{
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		SetCommMask(_handle, EV_RXCHAR);
		SetupComm(_handle, 1024, 1024);

		/* Configure port setting */
		if (!GetCommConfig(_handle, &(_commConfig), &confSize) || 
			!GetCommState(_handle, &(_commConfig.dcb)))
		{
			g_mutex_unlock(_mutex);
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
			_isMultiSetting = FALSE;
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (!SetCommConfig(_handle, &(_commConfig), sizeof(COMMCONFIG)))
		{
			_isMultiSetting = FALSE;
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		if (!SetCommTimeouts(_handle, &(_commTimeouts)))
		{
			_isMultiSetting = FALSE;
			g_mutex_unlock(_mutex);
			return GetLastError();
		}

		_isMultiSetting = FALSE;

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::close()
	{
		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}

		_bytesToWrite = 0;

		/* Force a flush */
		flush();
		CancelIo(_handle);
		if (CloseHandle(_handle))
		{
			_handle = INVALID_HANDLE_VALUE;
			g_mutex_unlock(_mutex);
			return JM_ERROR_SUCCESS;
		}
		g_mutex_unlock(_mutex);
		return JM_ERROR_GENERIC;
	}

	gint32 SerialPort::flush()
	{
		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (!FlushFileBuffers(_handle))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::discardInBuffer()
	{
		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (!PurgeComm(_handle, PURGE_RXABORT | PURGE_RXCLEAR))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::discardOutBuffer()
	{
		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		if (!PurgeComm(_handle, PURGE_TXABORT | PURGE_TXCLEAR))
		{
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	size_t SerialPort::bytesAvailable()
	{
		DWORD errors;
		COMSTAT status;

		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return 0;
		}

		if (!ClearCommError(_handle, &errors, &status))
		{
			g_mutex_unlock(_mutex);
			return 0;
		}

		g_mutex_unlock(_mutex);
		return (size_t)status.cbInQue;
	}

	gint32 SerialPort::setDtr(gboolean set)
	{
		DWORD dw_func;

		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
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
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	gint32 SerialPort::setRts(gboolean set)
	{
		DWORD dw_func;

		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
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
			g_mutex_unlock(_mutex);
			return JM_ERROR_GENERIC;
		}

		g_mutex_unlock(_mutex);
		return JM_ERROR_SUCCESS;
	}

	size_t SerialPort::read(guint8 *data, size_t count)
	{
		DWORD retVal = -1;

		g_return_val_if_fail(data != NULL, 0);
		g_return_val_if_fail(count > 0, 0);

		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return 0;
		}

		if (ReadFile(_handle, (void*)data, (DWORD)count, &retVal, NULL))
		{
			jm_log_write_hex(JM_LOG_DEBUG, "Serial Port: read", data, retVal);
		}		

		g_mutex_unlock(_mutex);
		return (size_t)retVal;
	}

	size_t SerialPort::write(const guint8 *data, size_t count)
	{
		DWORD retVal = -1;

		g_return_val_if_fail(data != NULL, 0);
		g_return_val_if_fail(count > 0, 0);

		g_mutex_lock(_mutex);

		if (!isOpen())
		{
			g_mutex_unlock(_mutex);
			return 0;
		}

		jm_log_write_hex(JM_LOG_DEBUG, "Serial Port: write", data, count);

		WriteFile(_handle, (const void*)data, (DWORD)count, &retVal, NULL);

		g_mutex_unlock(_mutex);
		return (size_t)retVal;
	}

	JMStringArray* SerialPort::getSystemPorts()
	{
		return enumerateDeviceWin(&GUID_DEVCLASS_PORTS);
	}

#endif
}
