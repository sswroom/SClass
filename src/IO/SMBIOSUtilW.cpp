#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/Library.h"
#include "IO/SMBIOSUtil.h"
#include "Text/MyString.h"
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

Optional<IO::SMBIOS> IO::SMBIOSUtil::GetSMBIOS()
{
	NN<IO::SMBIOS> smbios;
	IO::Library lib((const UTF8Char*)"kernel32.dll");
	UnsafeArray<UInt8> dataBuff;
	UnsafeArrayOpt<UInt8> optdataBuff = nullptr;
	UInt32 buffSize;
	GetSystemFirmwareTableFunc func = (GetSystemFirmwareTableFunc)lib.GetFunc("GetSystemFirmwareTable");
	if (func)
	{
		buffSize = func(*(UInt32*)"BMSR", 0, 0, 0);
		if (buffSize > 0)
		{
			dataBuff = MemAllocArr(UInt8, buffSize);
			func(*(UInt32*)"BMSR", 0, dataBuff.Ptr(), buffSize);
			UnsafeArray<const RawSMBIOSData> pDMIData = UnsafeArray<RawSMBIOSData>::ConvertFrom(dataBuff);
			NEW_CLASSNN(smbios, IO::SMBIOS(&pDMIData->SMBIOSTableData[0], pDMIData->Length, dataBuff));
			return smbios;
		}
	}

	UTF8Char sbuff[128];
	NN<Win32::WMIQuery> db;
	NN<DB::DBReader> r;
	NEW_CLASSNN(db, Win32::WMIQuery(L"ROOT\\WMI"));
	if (db->ExecuteReaderW(L"select * from MSSMBios_RawSMBiosTables").SetTo(r))
	{
		UIntOS i;
		UIntOS j;
		optdataBuff = nullptr;
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
						optdataBuff = dataBuff = MemAllocArr(UInt8, buffSize);
						r->GetBinary(i, dataBuff);
						break;
					}
				}
				i++;
			}
		}
		db->CloseReader(r);
	}
	db.Delete();
	if (optdataBuff.SetTo(dataBuff))
	{
		NEW_CLASSNN(smbios, IO::SMBIOS(dataBuff, buffSize, dataBuff));
		return smbios;
	}

	return nullptr;
}
