#include "Stdafx.h"
#include "Manage/CPUInfo.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/SMBIOS.h"
#include <sys/types.h>
#include <sys/processor.h>

typedef struct
{
	const UTF8Char *platformName;
} SystemInfoData;

IO::SystemInfo::SystemInfo()
{
	SystemInfoData *info = MemAlloc(SystemInfoData, 1);
	info->platformName = 0;
	this->clsData = info;
}

IO::SystemInfo::~SystemInfo()
{
	SystemInfoData *info = (SystemInfoData*)this->clsData;
	SDEL_TEXT(info->platformName);
	MemFree(info);
}

UTF8Char *IO::SystemInfo::GetPlatformName(UTF8Char *sbuff)
{
	SystemInfoData *info = (SystemInfoData*)this->clsData;
	if (info->platformName)
	{
		return Text::StrConcat(sbuff, info->platformName);
	}
	return 0;
}

UTF8Char *IO::SystemInfo::GetPlatformSN(UTF8Char *sbuff)
{
	SystemInfoData *data = (SystemInfoData*)this->clsData;
	return 0;
}

Int64 IO::SystemInfo::GetTotalMemSize()
{
	return 0;
}

Int64 IO::SystemInfo::GetTotalUsableMemSize()
{
	return 0;
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	return CT_DESKTOP;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	UOSInt retCnt = 0;
	RAMInfo *ram;
	WChar sbuff[128];
	Win32::SMBIOS *smbios = Win32::SMBIOS::GetSMBIOS();
	if (smbios)
	{
		Data::ArrayList<Win32::SMBIOS::MemoryDeviceInfo *> memList;
		Win32::SMBIOS::MemoryDeviceInfo *mem;
		Text::StringBuilderUTF8 sb;
		smbios->GetMemoryInfo(&memList);
		if (memList.GetCount() > 0)
		{
			UOSInt i = 0;
			UOSInt j = memList.GetCount();
			while (i < j)
			{
				mem = memList.GetItem(i);
				ram = MemAlloc(RAMInfo, 1);
				if (mem->deviceLocator)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->deviceLocator);
					ram->deviceLocator = Text::String::New(sb.ToCString());
				}
				else
				{
					ram->deviceLocator = 0;
				}
				if (mem->manufacturer)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->manufacturer);
					ram->manufacturer = Text::String::New(sb.ToCString());
				}
				else
				{
					ram->manufacturer = 0;
				}
				if (mem->partNo)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->partNo);
					ram->partNo = Text::String::New(sb.ToCString());
				}
				else
				{
					ram->partNo = 0;
				}
				if (mem->sn)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->sn);
					ram->sn = Text::String::New(sb.ToCString());
				}
				else
				{
					ram->sn = 0;
				}
				ram->defSpdMHz = mem->maxSpeedMTs;
				ram->confSpdMHz = mem->confSpeedMTs;
				ram->dataWidth = mem->dataWidthBits;
				ram->totalWidth = mem->totalWidthBits;
				ram->memorySize = mem->memorySize;
				ramList->Add(ram);
				retCnt++;

				i++;
			}

			smbios->FreeMemoryInfo(&memList);
			DEL_CLASS(smbios);
			return retCnt;
		}
		DEL_CLASS(smbios);
	}
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

