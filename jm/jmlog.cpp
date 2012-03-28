#include "jmlog.hpp"

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/log/trivial.hpp>

namespace JM
{
	Log::Log()
	{
	}

	Log::~Log()
	{
	}

	void Log::write(const std::string &str)
	{
		BOOST_LOG_TRIVIAL(trace) << str;
	}

	Log& Log::inst()
	{
		static Log instance;
		return instance;
	}

	void Log::write(const std::string &tag, const std::string &msg)
	{
		std::stringstream ss;
		ss << tag << " : " << msg;
		write(ss.str());
	}

	void Log::write(const std::string &tag, const boost::uint8_t *data, std::size_t count)
	{
		std::stringstream ss;
		ss << tag << " : ";
		for (std::size_t i = 0;i < count; i++)
		{
			ss << std::setw(2) << std::setfill('0') << std::hex << data[i] << " ";
		}
	}
}