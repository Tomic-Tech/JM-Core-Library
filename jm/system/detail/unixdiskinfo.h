/*
 * unixdiskinfo.hpp
 *
 *  Created on: 2012-4-26
 *      Author: Rocky
 */

#ifndef __JM_SYSTEM_DETAIL_UNIXDISKINFO_H__
#define __JM_SYSTEM_DETAIL_UNIXDISKINFO_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

namespace JM
{
namespace System
{
namespace Detail
{
class DiskInfo : public boost::noncopyable
{
private:
	DiskInfo()
	{

	}

	~DiskInfo()
	{

	}

public:
	static DiskInfo& inst()
	{
		static DiskInfo instance;
		return instance;
	}

	std::string model_number(size_t drive)
	{
		return std::string();
	}

	std::string serial_number(std::size_t drive)
	{
		return std::string();
	}

	std::string revision_number(std::size_t drive)
	{
		return std::string();
	}

	std::string drive_type(std::size_t drive)
	{
		return std::string();
	}

	boost::uint64_t drive_size(std::size_t drive)
	{
		return 0;
	}

	std::size_t load()
	{
		return 0;
	}
	std::size_t buffer_size(size_t drive)
	{
		return 0;
	}
};
}
}
}




#endif /* UNIXDISKINFO_HPP_ */
