#include "Stdafx.h"
#include "IO/Library.h"
#include "IO/Registry.h"
#include "IO/SystemInfo.h"
#include "Manage/CPUInfo.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/SMBIOSUtil.h"
#include "Win32/WMIQuery.h"
#include <windows.h>

IO::SystemInfo::SystemInfo()
{
	this->clsData = 0;
}

IO::SystemInfo::~SystemInfo()
{
}

UTF8Char *IO::SystemInfo::GetPlatformName(UTF8Char *sbuff)
{
	WChar wbuff[256];
	UTF8Char *ret = 0;

	IO::Registry *reg = IO::Registry::OpenLocalHardware();
	IO::Registry *reg2;
	if (reg)
	{
		reg2 = reg->OpenSubReg(L"DESCRIPTION\\System\\BIOS");
		if (reg2)
		{
			if (reg2->GetValueStr(L"SystemManufacturer", wbuff))
			{
				ret = Text::StrWChar_UTF8(sbuff, wbuff);
				*ret++ = ' ';
				reg2->GetValueStr(L"BaseBoardProduct", wbuff);
				ret = Text::StrWChar_UTF8(ret, wbuff);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		IO::Registry::CloseRegistry(reg);
	}

	if (ret == 0)
	{
		Win32::SMBIOS *smbios = Win32::SMBIOSUtil::GetSMBIOS();
		if (smbios)
		{
			ret = smbios->GetPlatformName(sbuff);
			DEL_CLASS(smbios);
			if (ret)
				return ret;
		}
	}
	return ret;
}

UTF8Char *IO::SystemInfo::GetPlatformSN(UTF8Char *sbuff)
{
	UTF8Char *ret = 0;
	Win32::SMBIOS *smbios = Win32::SMBIOSUtil::GetSMBIOS();
	if (smbios)
	{
		ret = smbios->GetPlatformSN(sbuff);
		DEL_CLASS(smbios);
		if (ret)
			return ret;
	}
	return 0;
}

UInt64 IO::SystemInfo::GetTotalMemSize()
{
	UInt64 totalSize = 0;

	UOSInt i;
	Data::ArrayList<RAMInfo*> ramList;
	this->GetRAMInfo(&ramList);
	i = ramList.GetCount();
	while (i-- > 0)
	{
		totalSize += ramList.GetItem(i)->memorySize;
	}
	this->FreeRAMInfo(&ramList);

	return totalSize;
}

UInt64 IO::SystemInfo::GetTotalUsableMemSize()
{
#if !defined(_WIN32_WCE)
	MEMORYSTATUSEX memStat;
	memStat.dwLength = sizeof(memStat);
	GlobalMemoryStatusEx(&memStat);
	return memStat.ullTotalPhys;
#else
	return 0;
#endif
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	Win32::SMBIOS *smbios = Win32::SMBIOSUtil::GetSMBIOS();
	Int32 ctype;
	if (smbios)
	{
		ctype = smbios->GetChassisType();
		DEL_CLASS(smbios);
		switch (ctype)
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
	Win32::WMIQuery *db;
	RAMInfo *ram;
	UTF8Char sbuff[128];
	DB::DBReader *r;
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


	NEW_CLASS(db, Win32::WMIQuery(L"ROOT\\CIMV2"));
//	r = db->ExecuteReader(L"select DeviceLocator, Manufacturer, PartNumber, SerialNumber, Speed from CIM_PhysicalMemory");
	r = db->ExecuteReader(L"select * from CIM_PhysicalMemory");
	if (r)
	{
		Text::StringBuilderUTF8 sb;
		UOSInt devLocCol = (UOSInt)-1;
		UOSInt manuCol = (UOSInt)-1;
		UOSInt partNoCol = (UOSInt)-1;
		UOSInt snCol = (UOSInt)-1;
		UOSInt spdCol = (UOSInt)-1;
		UOSInt dataWCol = (UOSInt)-1;
		UOSInt totalWCol = (UOSInt)-1;
		UOSInt i;
		i = r->ColCount();
		while (i-- > 0)
		{
			if (r->GetName(i, sbuff))
			{
				if (Text::StrEquals(sbuff, (const UTF8Char*)"DeviceLocator"))
				{
					devLocCol = i;
				}
				else if (Text::StrEquals(sbuff, (const UTF8Char*)"Manufacturer"))
				{
					manuCol = i;
				}
				else if (Text::StrEquals(sbuff, (const UTF8Char*)"PartNumber"))
				{
					partNoCol = i;
				}
				else if (Text::StrEquals(sbuff, (const UTF8Char*)"SerialNumber"))
				{
					snCol = i;
				}
				else if (Text::StrEquals(sbuff, (const UTF8Char*)"Speed"))
				{
					spdCol = i;
				}
				else if (Text::StrEquals(sbuff, (const UTF8Char*)"DataWidth"))
				{
					dataWCol = i;
				}
				else if (Text::StrEquals(sbuff, (const UTF8Char*)"TotalWidth"))
				{
					totalWCol = i;
				}
			}
		}

		if (devLocCol != (UOSInt)-1 && manuCol != (UOSInt)-1 && partNoCol != (UOSInt)-1 && snCol != (UOSInt)-1 && spdCol != (UOSInt)-1 && dataWCol != (UOSInt)-1 && totalWCol != (UOSInt)-1)
		{
			while (r->ReadNext())
			{
				ram = MemAlloc(RAMInfo, 1);
				sb.ClearStr();
				r->GetStr(devLocCol, &sb);
				ram->deviceLocator = Text::StrCopyNew(sb.ToString());
				sb.ClearStr();
				r->GetStr(manuCol, &sb);
				ram->manufacturer = Text::StrCopyNew(sb.ToString());
				sb.ClearStr();
				r->GetStr(partNoCol, &sb);
				ram->partNo = Text::StrCopyNew(sb.ToString());
				sb.ClearStr();
				r->GetStr(snCol, &sb);
				ram->sn = Text::StrCopyNew(sb.ToString());
				ram->defSpdMHz = (UInt32)r->GetInt32(spdCol);
				ram->confSpdMHz = 0;
				ram->dataWidth = (UInt32)r->GetInt32(dataWCol);
				ram->totalWidth = (UInt32)r->GetInt32(totalWCol);
				ram->memorySize = 0;
				ramList->Add(ram);
				retCnt++;
			}
		}
		db->CloseReader(r);
	}

	DEL_CLASS(db);
	return retCnt;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	UOSInt i;
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
