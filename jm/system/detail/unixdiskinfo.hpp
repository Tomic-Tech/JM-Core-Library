/*
 * unixdiskinfo.hpp
 *
 *  Created on: 2012-4-26
 *      Author: Rocky
 */

#ifndef __JM_SYSTEM_DETAIL_UNIXDISKINFO_HPP__
#define __JM_SYSTEM_DETAIL_UNIXDISKINFO_HPP__

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

	std::string modelNumber(size_t drive)
	{
		return std::string();
	}

	std::string serialNumber(std::size_t drive)
	{
		return std::string();
	}

	std::string revisionNumber(std::size_t drive)
	{
		return std::string();
	}

	std::string driveType(std::size_t drive)
	{
		return std::string();
	}

	boost::uint64_t driveSize(std::size_t drive)
	{
		return 0;
	}

	std::size_t load()
	{
		return 0;
	}
	std::size_t bufferSize(size_t drive)
	{
		return 0;
	}
};
}
}
}




#endif /* UNIXDISKINFO_HPP_ */
