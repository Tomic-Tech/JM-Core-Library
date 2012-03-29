#include "jmserialport.hpp"
#include "jmlog.hpp"

namespace JM
{
	template<typename DurationType>
	boost::int32_t SerialPort::setWriteTimeout(DurationType const &time)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);

		_writeTimeout = time;

		if (!isOpen())
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (time.total_milliseconds() == 0)
		{
			_commTimeouts.WriteTotalTimeoutConstant = MAXDWORD;
			_commTimeouts.WriteTotalTimeoutMultiplier = MAXDWORD;
		}
		else
		{
			_commTimeouts.WriteTotalTimeoutConstant = (DWORD)time.total_milliseconds();
			_commTimeouts.WriteTotalTimeoutMultiplier = 0;
		}

		if (_isMultiSetting)
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommTimeouts(_handle, &(_commTimeouts)))
		{
			
			return GetLastError();
		}

		
		return JM_ERROR_SUCCESS;
	}

	template<typename DurationType>
	boost::int32_t SerialPort::setReadTimeout(DurationType const &time)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);

		_readTimeout = time;

		if (!isOpen())
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (time.total_milliseconds() == 0)
		{
			_commTimeouts.ReadIntervalTimeout = MAXDWORD;
			_commTimeouts.ReadTotalTimeoutConstant = 0;
		}
		else
		{
			_commTimeouts.ReadIntervalTimeout = (DWORD)time.total_milliseconds();
			_commTimeouts.ReadTotalTimeoutConstant = (DWORD)time.total_milliseconds();
		}

		_commTimeouts.ReadTotalTimeoutMultiplier = 0;

		if (_isMultiSetting)
		{
			
			return JM_ERROR_SUCCESS;
		}

		if (!SetCommTimeouts(_handle, &(_commTimeouts)))
		{
			
			return GetLastError();
		}

		
		return JM_ERROR_SUCCESS;
	}

	template<typename MutableBufferSequence>
	size_t SerialPort::read(const MutableBufferSequence &data)
	{
		DWORD retVal = -1;

		boost::unique_lock<boost::mutex> lock(_mutex);

		if (!isOpen())
		{
			
			return 0;
		}

		boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);
		std::size_t count = boost::asio::buffer_size(data);

		if (ReadFile(_handle, (void*)p, (DWORD)count, &retVal, NULL))
		{
			JM::Log::inst().write("Serial Port: read", p, retVal);
		}		

		
		return (size_t)retVal;
	}

	template<typename ConstBufferSequence>
	size_t SerialPort::write(const ConstBufferSequence &data)
	{
		DWORD retVal = -1;

		boost::unique_lock<boost::mutex> lock(_mutex);

		if (!isOpen())
		{
			
			return 0;
		}

		const boost::uint8_t *p = boost::asio::buffer_cast<const boost::uint8_t*>(data);
		std::size_t count = boost::asio::buffer_size(data);
		JM::Log::inst().write("Serial Port: write", p, count);

		WriteFile(_handle, (const void*)p, (DWORD)count, &retVal, NULL);

		
		return (size_t)retVal;
	}
}
