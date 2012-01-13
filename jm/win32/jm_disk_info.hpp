#ifndef JM_DISK_INFO_WIN32_HPP
#define JM_DISK_INFO_WIN32_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jm_types.hpp>

#ifdef JM_OS_WIN
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <Windows.h>
#include <WinIoCtl.h>

namespace jm {

namespace win32 {

typedef std::vector<WORD*> LISTDATA;

class DiskInfo {
	static DiskInfo* m_instance;
	static char HardDriveSerialNumber [1024];
	LISTDATA m_list;
public:
	static DiskInfo& GetDiskInfo();
	~DiskInfo(void);
	BOOL ReadIdeDriveAsScsiDriveInNT (void);
	long LoadDiskInfo ();
	int ReadPhysicalDriveInNTWithAdminRights (void);
	int ReadPhysicalDriveInNTWithZeroRights (void);
	BOOL ReadDrivePortsInWin9X (void);
	int ReadPhysicalDriveInNTUsingSmart (void);

	unsigned __int64 DriveSize (UINT drive);
	UINT GetCount (){return m_list.size();};
	UINT BufferSize (UINT drive);
	std::string ModelNumber (UINT drive);
	std::string SerialNumber (UINT drive);
	std::string RevisionNumber (UINT drive);
	std::string DriveType (UINT drive);
private:
	DiskInfo(void);
	static std::string flipAndCodeBytes (const std::string &str);
	static std::string ConvertToString (WORD diskdata [256], int firstIndex, int lastIndex);

	static BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned);
	BOOL AddIfNew(USHORT *pIdSector);
};

}
}

#endif

#endif
