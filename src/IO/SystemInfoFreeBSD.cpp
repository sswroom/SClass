#include "Stdafx.h"
#include "Manage/CPUInfo.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Win32/SMBIOS.h"
#include "Win32/SMBIOSUtil.h"
#include <sys/types.h>
#include <sys/sysctl.h>

struct IO::SystemInfo::ClassData
{
	Text::String *cpuName;
	Text::String *platformName;
};

Bool SystemInfo_ReadFile(const UTF8Char *fileName, Text::StringBuilderUTF8 *sb)
{
	Bool succ = false;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(&fs));
		while (reader.ReadLine(sb, 512))
		{
		}
		succ = true;
	}
	return succ;
}

IO::SystemInfo::SystemInfo()
{
	ClassData *info = MemAlloc(ClassData, 1);
	info->platformName = 0;

	Text::StringBuilderUTF8 sb;
	Char sbuff[1024];
#if defined(__APPLE__)
	int mib[2];
	size_t size;

	mib[0] = CTL_HW;
	mib[1] = HW_MODEL;
	size = 1024;
	if (sysctl(mib, 2, sbuff, &size, 0, 0) == 0)
	{
		sb.ClearStr();
		sb.AppendC((const UTF8Char*)sbuff, size);
		info->platformName = Text::String::New(sb.ToString(), sb.GetLength());
	}
#endif

	this->clsData = info;
}

IO::SystemInfo::~SystemInfo()
{
	SDEL_STRING(this->clsData->platformName);
	MemFree(this->clsData);
}

UTF8Char *IO::SystemInfo::GetPlatformName(UTF8Char *sbuff)
{
	if (this->clsData->platformName)
	{
		return this->clsData->platformName->ConcatTo(sbuff);
	}
	return 0;
}

UTF8Char *IO::SystemInfo::GetPlatformSN(UTF8Char *sbuff)
{
	return 0;
}

UInt64 IO::SystemInfo::GetTotalMemSize()
{
#if defined(__APPLE__)
	UInt64 memSize;
	size_t len;
	len = sizeof(memSize);
	memSize = 0;
	sysctlbyname("hw.memsize", &memSize, &len, 0, 0);
	return memSize;
#else
	UInt64 memSize;
	size_t len;
	len = sizeof(memSize);
	memSize = 0;
	sysctlbyname("hw.realmem", &memSize, &len, 0, 0);
	return memSize;
#endif
}

UInt64 IO::SystemInfo::GetTotalUsableMemSize()
{
#if defined(__APPLE__)
	UInt64 memSize;
	size_t len;
	len = sizeof(memSize);
	sysctlbyname("hw.memsize", &memSize, &len, 0, 0);
	return memSize;
#else
	int mib[2];
	UInt64 memSize;
	size_t len;
	mib[0] = CTL_HW;
#if defined(HW_PHYSMEM64)
	mib[1] = HW_PHYSMEM64;
#else
	mib[1] = HW_PHYSMEM;
#endif
	memSize = 0;
	len = sizeof(memSize);
	sysctl(mib, 2, &memSize, &len, 0, 0);
	return memSize;
#endif
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	return CT_DESKTOP;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	UOSInt retCnt = 0;
	RAMInfo *ram;
	Win32::SMBIOS *smbios = Win32::SMBIOSUtil::GetSMBIOS();
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
					sb.AppendSlow((const UTF8Char*)mem->deviceLocator);
					ram->deviceLocator = Text::String::New(sb.ToCString());
				}
				else
				{
					ram->deviceLocator = 0;
				}
				if (mem->manufacturer)
				{
					sb.ClearStr();
					sb.AppendSlow((const UTF8Char*)mem->manufacturer);
					ram->manufacturer = Text::String::New(sb.ToCString());
				}
				else
				{
					ram->manufacturer = 0;
				}
				if (mem->partNo)
				{
					sb.ClearStr();
					sb.AppendSlow((const UTF8Char*)mem->partNo);
					ram->partNo = Text::String::New(sb.ToCString());
				}
				else
				{
					ram->partNo = 0;
				}
				if (mem->sn)
				{
					sb.ClearStr();
					sb.AppendSlow((const UTF8Char*)mem->sn);
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

