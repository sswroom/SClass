#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SMBIOSUtil.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#else
#include <sys/sysinfo.h>
#endif

struct IO::SystemInfo::ClassData
{
	Text::String *platformName;
	Text::String *platformSN;
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
	info->platformName = 0;
	info->platformSN = 0;

	Text::StringBuilderUTF8 sb;
	if (SystemInfo_ReadFile(CSTR("/sys/class/dmi/id/board_vendor"), sb))
	{
		sb.AppendC(UTF8STRC(" "));
		if (!SystemInfo_ReadFile(CSTR("/sys/class/dmi/id/board_name"), sb))
		{
			sb.RemoveChars(1);
		}
		info->platformName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	else if (SystemInfo_ReadFile(CSTR("/sys/class/dmi/id/sys_vendor"), sb))
	{
		sb.AppendC(UTF8STRC(" "));
		if (!SystemInfo_ReadFile(CSTR("/sys/class/dmi/id/product_name"), sb))
		{
			sb.RemoveChars(1);
		}
		info->platformName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}


	sb.ClearStr();
	if (SystemInfo_ReadFile(CSTR("/sys/class/dmi/id/board_serial"), sb))
	{
		info->platformSN = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	else if (SystemInfo_ReadFile(CSTR("/sys/class/dmi/id/product_serial"), sb))
	{
		info->platformSN = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}

	this->clsData = info;
}

IO::SystemInfo::~SystemInfo()
{
	SDEL_STRING(this->clsData->platformName);
	SDEL_STRING(this->clsData->platformSN);
	MemFreeNN(this->clsData);
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformName(UnsafeArray<UTF8Char> sbuff)
{
	if (this->clsData->platformName)
	{
		return this->clsData->platformName->ConcatTo(sbuff);
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformSN(UnsafeArray<UTF8Char> sbuff)
{
	if (this->clsData->platformSN)
	{
		return this->clsData->platformSN->ConcatTo(sbuff);
	}
	return nullptr;
}

UInt64 IO::SystemInfo::GetTotalMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (UInt64)info.mem_unit;
	}
	return 0;
}

UInt64 IO::SystemInfo::GetTotalUsableMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (UInt64)info.mem_unit;
	}
	return 0;
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	Text::StringBuilderUTF8 sb;
	if (SystemInfo_ReadFile(CSTR("/sys/class/dmi/id/chassis_type"), sb))
	{
		switch (Text::StrToInt32(sb.ToString()))
		{
		case 3:
			return CT_DESKTOP;
		case 4:
			return CT_LPDESKTOP;
		case 5:
			return CT_PIZZABOX;
		case 6:
			return CT_MINITOWER;
		case 7:
			return CT_TOWER;
		case 8:
			return CT_PORTABLE;
		case 9:
			return CT_LAPTOP;
		case 10:
			return CT_NOTEBOOK;
		case 11:
			return CT_HANDHELD;
		case 12:
			return CT_DOCKINGSTATION;
		case 13:
			return CT_AIO;
		case 14:
			return CT_SUBNOTEBOOk;
		case 15:
			return CT_SPACESAVING;
		case 16:
			return CT_LUNCHBOX;
		case 17:
			return CT_MAINSERVER;
		case 18:
			return CT_EXPANSION;
		case 19:
			return CT_SUBCHASSIS;
		case 20:
			return CT_BUSEXPANSION;
		case 21:
			return CT_PERIPHERAL;
		case 22:
			return CT_RAID;
		case 23:
			return CT_RACKMOUNT;
		case 24:
			return CT_SEALED_CASE_PC;
		case 25:
			return CT_MULTISYSTEM;
		case 26:
			return CT_COMPACT_PCI;
		case 27:
			return CT_ADVANCEDTCA;
		case 28:
			return CT_BLADE;
		case 29:
			return CT_BLADEENCLOSURE;
		case 30:
			return CT_TABLET;
		case 31:
			return CT_CONVERTIBLE;
		case 32:
			return CT_DETACHABLE;
		case 33:
			return CT_IOT_GATEWAY;
		case 34:
			return CT_EMBEDDEDPC;
		case 35:
			return CT_MINI_PC;
		case 36:
			return CT_STICK_PC;
		default:
			return CT_UNKNOWN;
		}
	}
	return CT_UNKNOWN;
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
		smbios->GetMemoryInfo(memList);
		if (memList.GetCount() > 0)
		{
			UIntOS i = 0;
			UIntOS j = memList.GetCount();
			while (i < j)
			{
				mem = memList.GetItemNoCheck(i);
				ram = MemAllocNN(RAMInfo);
				ram->deviceLocator = Text::String::NewOrNullSlow(UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->deviceLocator));
				ram->manufacturer = Text::String::NewOrNullSlow(UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->manufacturer));
				ram->partNo = Text::String::NewOrNullSlow(UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->partNo));
				ram->sn = Text::String::NewOrNullSlow(UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->sn));
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
	UIntOS i;
	NN<RAMInfo> ram;
	i = ramList->GetCount();
	while (i-- > 0)
	{
		ram = ramList->GetItemNoCheck(i);
		OPTSTR_DEL(ram->deviceLocator);
		OPTSTR_DEL(ram->manufacturer);
		OPTSTR_DEL(ram->partNo);
		OPTSTR_DEL(ram->sn);
		MemFreeNN(ram);
	}
}
