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

Bool SystemInfo_ReadFile(Text::CString fileName, NotNullPtr<Text::StringBuilderUTF8> sb)
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
	ClassData *info = MemAlloc(ClassData, 1);
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
	if (this->clsData->platformSN)
	{
		return this->clsData->platformSN->ConcatTo(sbuff);
	}
	return 0;
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

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	UOSInt retCnt = 0;
	RAMInfo *ram;
	IO::SMBIOS *smbios = IO::SMBIOSUtil::GetSMBIOS();
	if (smbios)
	{
		Data::ArrayList<IO::SMBIOS::MemoryDeviceInfo *> memList;
		IO::SMBIOS::MemoryDeviceInfo *mem;
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
					ram->deviceLocator = Text::String::New(sb.ToCString()).Ptr();
				}
				else
				{
					ram->deviceLocator = 0;
				}
				if (mem->manufacturer)
				{
					sb.ClearStr();
					sb.AppendSlow((const UTF8Char*)mem->manufacturer);
					ram->manufacturer = Text::String::New(sb.ToCString()).Ptr();
				}
				else
				{
					ram->manufacturer = 0;
				}
				if (mem->partNo)
				{
					sb.ClearStr();
					sb.AppendSlow((const UTF8Char*)mem->partNo);
					ram->partNo = Text::String::New(sb.ToCString()).Ptr();
				}
				else
				{
					ram->partNo = 0;
				}
				if (mem->sn)
				{
					sb.ClearStr();
					sb.AppendSlow((const UTF8Char*)mem->sn);
					ram->sn = Text::String::New(sb.ToCString()).Ptr();
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
	UOSInt i;
	RAMInfo *ram;
	i = ramList->GetCount();
	while (i-- > 0)
	{
		ram = ramList->GetItem(i);
		SDEL_STRING(ram->deviceLocator);
		SDEL_STRING(ram->manufacturer);
		SDEL_STRING(ram->partNo);
		SDEL_STRING(ram->sn);
		MemFree(ram);
	}
}
