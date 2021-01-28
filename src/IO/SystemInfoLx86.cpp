#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Win32/SMBIOSUtil.h"
#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#else
#include <sys/sysinfo.h>
#endif

typedef struct
{
	const UTF8Char *platformName;
	const UTF8Char *platformSN;
} SystemInfoData;

Bool SystemInfo_ReadFile(const UTF8Char *fileName, Text::StringBuilderUTF *sb)
{
	Bool succ = false;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		while (reader->ReadLine(sb, 512))
		{
		}
		DEL_CLASS(reader);
		succ = true;
	}
	DEL_CLASS(fs);
	return succ;
}

IO::SystemInfo::SystemInfo()
{
	SystemInfoData *info = MemAlloc(SystemInfoData, 1);
	info->platformName = 0;
	info->platformSN = 0;

	Text::StringBuilderUTF8 sb;
	if (SystemInfo_ReadFile((const UTF8Char*)"/sys/class/dmi/id/board_vendor", &sb))
	{
		sb.Append((const UTF8Char*)" ");
		if (!SystemInfo_ReadFile((const UTF8Char*)"/sys/class/dmi/id/board_name", &sb))
		{
			sb.RemoveChars(1);
		}
		info->platformName = Text::StrCopyNew(sb.ToString());
	}
	else if (SystemInfo_ReadFile((const UTF8Char*)"/sys/class/dmi/id/sys_vendor", &sb))
	{
		sb.Append((const UTF8Char*)" ");
		if (!SystemInfo_ReadFile((const UTF8Char*)"/sys/class/dmi/id/product_name", &sb))
		{
			sb.RemoveChars(1);
		}
		info->platformName = Text::StrCopyNew(sb.ToString());
	}


	sb.ClearStr();
	if (SystemInfo_ReadFile((const UTF8Char*)"/sys/class/dmi/id/board_serial", &sb))
	{
		info->platformSN = Text::StrCopyNew(sb.ToString());
	}
	else if (SystemInfo_ReadFile((const UTF8Char*)"/sys/class/dmi/id/product_serial", &sb))
	{
		info->platformSN = Text::StrCopyNew(sb.ToString());
	}

	this->clsData = info;
}

IO::SystemInfo::~SystemInfo()
{
	SystemInfoData *info = (SystemInfoData*)this->clsData;
	SDEL_TEXT(info->platformName);
	SDEL_TEXT(info->platformSN);
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
	SystemInfoData *info = (SystemInfoData*)this->clsData;
	if (info->platformSN)
	{
		return Text::StrConcat(sbuff, info->platformSN);
	}
	return 0;
}

Int64 IO::SystemInfo::GetTotalMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (Int64)info.mem_unit;
	}
	return 0;
}

Int64 IO::SystemInfo::GetTotalUsableMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (Int64)info.mem_unit;
	}
	return 0;
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	Text::StringBuilderUTF8 sb;
	if (SystemInfo_ReadFile((const UTF8Char*)"/sys/class/dmi/id/chassis_type", &sb))
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

OSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	OSInt retCnt = 0;
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
			OSInt i = 0;
			OSInt j = memList.GetCount();
			while (i < j)
			{
				mem = memList.GetItem(i);
				ram = MemAlloc(RAMInfo, 1);
				if (mem->deviceLocator)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->deviceLocator);
					ram->deviceLocator = Text::StrCopyNew(sb.ToString());
				}
				else
				{
					ram->deviceLocator = 0;
				}
				if (mem->manufacturer)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->manufacturer);
					ram->manufacturer = Text::StrCopyNew(sb.ToString());
				}
				else
				{
					ram->manufacturer = 0;
				}
				if (mem->partNo)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->partNo);
					ram->partNo = Text::StrCopyNew(sb.ToString());
				}
				else
				{
					ram->partNo = 0;
				}
				if (mem->sn)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)mem->sn);
					ram->sn = Text::StrCopyNew(sb.ToString());
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
	OSInt i;
	RAMInfo *ram;
	i = ramList->GetCount();
	while (i-- > 0)
	{
		ram = ramList->GetItem(i);
		SDEL_TEXT(ram->deviceLocator);
		SDEL_TEXT(ram->manufacturer);
		SDEL_TEXT(ram->partNo);
		SDEL_TEXT(ram->sn);
		MemFree(ram);
	}
}
