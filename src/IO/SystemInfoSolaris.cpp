#include "Stdafx.h"
#include "Manage/CPUInfo.h"
#include "IO/FileStream.h"
#include "IO/SMBIOSUtil.h"
#include "IO/StreamReader.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include <sys/types.h>
#include <sys/processor.h>

struct IO::SystemInfo::ClassData
{
	UnsafeArrayOpt<const UTF8Char> platformName;
};

IO::SystemInfo::SystemInfo()
{
	NN<ClassData> info = MemAllocNN(ClassData);
	info->platformName = nullptr;
	this->clsData = info;
}

IO::SystemInfo::~SystemInfo()
{
	NN<ClassData> info = this->clsData;
	SDEL_TEXT(info->platformName);
	MemFree(info);
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformName(UnsafeArray<UTF8Char> sbuff)
{
	NN<ClassData> info = this->clsData;
	UnsafeArra<const UTF8Char> platformName;
	if (info->platformName.SetTo(platformName))
	{
		return Text::StrConcat(sbuff, platformName);
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformSN(UnsafeArray<UTF8Char> sbuff)
{
	return nullptr;
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

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	UIntOS retCnt = 0;
	NN<RAMInfo> ram;
	NN<IO::SMBIOS> smbios;
	if (IO::SMBIOSUtil::GetSMBIOS().SetTo(smbios))
	{
		Data::ArrayListNN<IO::SMBIOS::MemoryDeviceInfo> memList;
		NN<IO::SMBIOS::MemoryDeviceInfo> mem;
		UnsafeArray<const UTF8Char> s;
		smbios->GetMemoryInfo(memList);
		if (memList.GetCount() > 0)
		{
			UIntOS i = 0;
			UIntOS j = memList.GetCount();
			while (i < j)
			{
				mem = memList.GetItemNoCheck(i);
				ram = MemAllocNN(RAMInfo);
				if (mem->deviceLocator.SetTo(s))
				{
					ram->deviceLocator = Text::String::NewSlow(s);
				}
				else
				{
					ram->deviceLocator = nullptr;
				}
				if (mem->manufacturer.SetTo(s))
				{
					ram->manufacturer = Text::String::NewSlow(s);
				}
				else
				{
					ram->manufacturer = nullptr;
				}
				if (mem->partNo.SetTo(s))
				{
					ram->partNo = Text::String::NewSlow(s);
				}
				else
				{
					ram->partNo = nullptr;
				}
				if (mem->sn.SetTo(s))
				{
					ram->sn = Text::String::NewSlow(s);
				}
				else
				{
					ram->sn = nullptr;
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

			smbios->FreeMemoryInfo(memList);
			smbios.Delete();
			return retCnt;
		}
		smbios.Delete();
	}
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	ramList->MemFreeAll();
}

