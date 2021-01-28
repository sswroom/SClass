#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Library.h"
#include "Text/MyString.h"
#include "Win32/SMBIOSUtil.h"
#include "Win32/WMIQuery.h"
#include <windows.h>

typedef UINT (WINAPI *GetSystemFirmwareTableFunc)(DWORD FirmwareTableProviderSignature, DWORD FirmwareTableID, PVOID pFirmwareTableBuffer, DWORD BufferSize);
struct RawSMBIOSData
{
    BYTE    Used20CallingMethod;
    BYTE    SMBIOSMajorVersion;
    BYTE    SMBIOSMinorVersion;
    BYTE    DmiRevision;
    DWORD    Length;
    BYTE    SMBIOSTableData[1];
};

Win32::SMBIOS *Win32::SMBIOSUtil::GetSMBIOS()
{
	Win32::SMBIOS *smbios;
	IO::Library lib((const UTF8Char*)"kernel32.dll");
	UInt8 *dataBuff = 0;
	UInt32 buffSize;
	GetSystemFirmwareTableFunc func = (GetSystemFirmwareTableFunc)lib.GetFunc("GetSystemFirmwareTable");
	if (func)
	{
		buffSize = func(*(UInt32*)"BMSR", 0, 0, 0);
		if (buffSize > 0)
		{
			dataBuff = MemAlloc(UInt8, buffSize);
			func(*(UInt32*)"BMSR", 0, dataBuff, buffSize);
			const RawSMBIOSData *pDMIData = (RawSMBIOSData *)dataBuff;
			NEW_CLASS(smbios, Win32::SMBIOS(&pDMIData->SMBIOSTableData[0], pDMIData->Length, dataBuff));
			return smbios;
		}
	}

	UTF8Char sbuff[128];
	Win32::WMIQuery *db;
	DB::DBReader *r;
	NEW_CLASS(db, Win32::WMIQuery(L"ROOT\\WMI"));
	r = db->ExecuteReader(L"select * from MSSMBios_RawSMBiosTables");
	if (r)
	{
		OSInt i;
		OSInt j;
		dataBuff = 0;
		buffSize = 0;
		if (r->ReadNext())
		{
			i = 0;
			j = r->ColCount();
			while (i < j)
			{
				r->GetName(i, sbuff);
				if (Text::StrEquals(sbuff, (const UTF8Char*)"SMBiosData"))
				{
					buffSize = (UInt32)r->GetBinarySize(i);
					if (buffSize > 0)
					{
						dataBuff = MemAlloc(UInt8, buffSize);
						r->GetBinary(i, dataBuff);
						break;
					}
				}
				i++;
			}
		}
		db->CloseReader(r);
	}
	DEL_CLASS(db);
	if (dataBuff)
	{
		NEW_CLASS(smbios, Win32::SMBIOS(dataBuff, buffSize, dataBuff));
		return smbios;
	}

	return 0;
}
