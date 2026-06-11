#include "Stdafx.h"
#include "Manage/CPUInfo.h"
#include "IO/FileStream.h"
#include "IO/SMBIOS.h"
#include "IO/SMBIOSUtil.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#if defined(__APPLE__)
#undef UTF8Char
#undef UTF16Char
#undef UTF32Char
#include <stdlib.h>
#include <IOKit/IOKitLib.h>
#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch
#endif

struct IO::SystemInfo::ClassData
{
	Optional<Text::String> cpuName;
	Optional<Text::String> platformName;
};

Bool SystemInfo_ReadFile(Text::CStringNN fileName, NN<Text::StringBuilderUTF8> sb)
{
	Bool succ = false;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(fs);
		while (reader.ReadLine(sb, 512))
		{
		}
		succ = true;
	}
	return succ;
}

IO::SystemInfo::SystemInfo()
{
	NN<ClassData> info = MemAllocNN(ClassData);
	info->platformName = nullptr;
	info->cpuName = nullptr;

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
		info->platformName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
#endif

	this->clsData = info;
}

IO::SystemInfo::~SystemInfo()
{
	OPTSTR_DEL(this->clsData->platformName);
	OPTSTR_DEL(this->clsData->cpuName);
	MemFreeNN(this->clsData);
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformName(UnsafeArray<UTF8Char> sbuff)
{
	NN<Text::String> s;
	if (this->clsData->platformName.SetTo(s))
	{
		return s->ConcatTo(sbuff);
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformSN(UnsafeArray<UTF8Char> sbuff)
{
#if defined(__APPLE__)
	io_service_t platformExpert = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                                         IOServiceMatching("IOPlatformExpertDevice"));

	if (platformExpert) {
		CFTypeRef serialNumberAsCFString = 
			IORegistryEntryCreateCFProperty(platformExpert,
											CFSTR(kIOPlatformSerialNumberKey),
											kCFAllocatorDefault, 0);
		if (serialNumberAsCFString)
		{
			CFStringRef sn = (CFStringRef)serialNumberAsCFString;
			if (CFStringGetCString(sn, (char*)sbuff, 128, kCFStringEncodingUTF8))
			{
				sbuff += Text::StrCharCnt(sbuff);
			}
			else
			{
				sbuff = 0;
			}
		}
		else
		{
			sbuff = 0;
		}

		IOObjectRelease(platformExpert);
		return sbuff;
	}
#endif
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

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	UIntOS retCnt = 0;
	NN<RAMInfo> ram;
	NN<IO::SMBIOS> smbios;
	if (IO::SMBIOSUtil::GetSMBIOS().SetTo(smbios))
	{
		Data::ArrayList<IO::SMBIOS::MemoryDeviceInfo *> memList;
		IO::SMBIOS::MemoryDeviceInfo *mem;
		Text::StringBuilderUTF8 sb;
		smbios->GetMemoryInfo(&memList);
		if (memList.GetCount() > 0)
		{
			UIntOS i = 0;
			UIntOS j = memList.GetCount();
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
			smbios.Delete();
			return retCnt;
		}
		smbios.Delete();
	}
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	UIntOS i = ramList->GetCount();
	while (i-- > 0)
	{
		NN<RAMInfo> ram;
		if (ramList->GetItem(i).SetTo(ram))
		{
			OPTSTR_DEL(ram->deviceLocator);
			OPTSTR_DEL(ram->manufacturer);
			OPTSTR_DEL(ram->partNo);
			OPTSTR_DEL(ram->sn);
			MemFreeNN(ram);
		}
	}
	ramList->Clear();
}

