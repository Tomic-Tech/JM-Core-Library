#ifndef __JM_DISK_INFO_WIN32_HPP__
#define __JM_DISK_INFO_WIN32_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <Windows.h>
#include <WinIoCtl.h>
#include "jmutils.h"

#include <vector>
#include <string>
#include <boost/cstdint.hpp>

#ifdef BOOST_WINDOWS

namespace JM
{
	namespace Win32
	{
		class DiskInfo
		{
		public:
			static DiskInfo& inst();
			std::string modelNumber(std::size_t drive);
			std::string serialNumber(std::size_t drive);
			std::string revisionNumber(std::size_t drive);
			std::string driveType(std::size_t drive);
			boost::uint64_t driveSize(std::size_t drive);
			std::size_t load();
			std::size_t bufferSize(std::size_t drive);
		private:
			DiskInfo();
			~DiskInfo();

			// DoIDENTIFY
			// FUNCTION: Send an IDENTIFY command to the drive
			// bDriveNum = 0-3
			// bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
			BOOL DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned);
			BOOL AddIfNew(USHORT *pIdSector);
			//  function to decode the serial numbers of IDE hard drives
			//  using the IOCTL_STORAGE_QUERY_PROPERTY command 
			std::string flipAndCodeBytes(const std::string &str);
			std::string ConvertToString(WORD diskdata [256], int firstIndex, int lastIndex);
			BOOL ReadPhysicalDriveInNTUsingSmart();
			BOOL ReadPhysicalDriveInNTWithAdminRights();
			BOOL ReadPhysicalDriveInNTWithZeroRights();
			//  ---------------------------------------------------
			BOOL ReadDrivePortsInWin9X();
			BOOL ReadIdeDriveAsScsiDriveInNT();
		private:
			char HardDriveSerialNumber[1024];
			std::vector<PWORD> m_list;
		};
	}
}

#endif

#endif
