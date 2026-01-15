#include "Stdafx.h"
#include "IO/Library.h"
#include "IO/Registry.h"
#include "IO/SMBIOSUtil.h"
#include "IO/SystemInfo.h"
#include "Manage/CPUInfo.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WMIQuery.h"
#include <windows.h>

IO::SystemInfo::SystemInfo()
{
}

IO::SystemInfo::~SystemInfo()
{
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformName(UnsafeArray<UTF8Char> sbuff)
{
	WChar wbuff[256];
	UnsafeArrayOpt<UTF8Char> ret = 0;
	UnsafeArray<UTF8Char> nnret;

	NN<IO::Registry> reg;
	NN<IO::Registry> reg2;
	if (IO::Registry::OpenLocalHardware().SetTo(reg))
	{
		if (reg->OpenSubReg(L"DESCRIPTION\\System\\BIOS").SetTo(reg2))
		{
			if (reg2->GetValueStr(L"SystemManufacturer", wbuff).NotNull())
			{
				nnret = Text::StrWChar_UTF8(sbuff, wbuff);
				*nnret++ = ' ';
				reg2->GetValueStr(L"BaseBoardProduct", wbuff);
				ret = Text::StrWChar_UTF8(nnret, wbuff);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		IO::Registry::CloseRegistry(reg);
	}

	if (ret == 0)
	{
		NN<IO::SMBIOS> smbios;
		if (IO::SMBIOSUtil::GetSMBIOS().SetTo(smbios))
		{
			ret = smbios->GetPlatformName(sbuff);
			smbios.Delete();
			if (ret.SetTo(nnret))
				return nnret;
		}
	}
	return ret;
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformSN(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArrayOpt<UTF8Char> ret = 0;
	NN<IO::SMBIOS> smbios;
	if (IO::SMBIOSUtil::GetSMBIOS().SetTo(smbios))
	{
		ret = smbios->GetPlatformSN(sbuff);
		smbios.Delete();
		if (ret.NotNull())
			return ret;
	}
	return 0;
}

UInt64 IO::SystemInfo::GetTotalMemSize()
{
	UInt64 totalSize = 0;

	UIntOS i;
	Data::ArrayListNN<RAMInfo> ramList;
	this->GetRAMInfo(ramList);
	i = ramList.GetCount();
	while (i-- > 0)
	{
		totalSize += ramList.GetItemNoCheck(i)->memorySize;
	}
	this->FreeRAMInfo(ramList);

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
	NN<IO::SMBIOS> smbios;
	Int32 ctype;
	if (IO::SMBIOSUtil::GetSMBIOS().SetTo(smbios))
	{
		ctype = smbios->GetChassisType();
		smbios.Delete();
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

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	UIntOS retCnt = 0;
	Win32::WMIQuery *db;
	NN<RAMInfo> ram;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> csptr;
	NN<DB::DBReader> r;
	NN<IO::SMBIOS> smbios;
	if (IO::SMBIOSUtil::GetSMBIOS().SetTo(smbios))
	{
		Data::ArrayListNN<IO::SMBIOS::MemoryDeviceInfo> memList;
		NN<IO::SMBIOS::MemoryDeviceInfo> mem;
		Text::StringBuilderUTF8 sb;
		smbios->GetMemoryInfo(memList);
		if (memList.GetCount() > 0)
		{
			UIntOS i = 0;
			UIntOS j = memList.GetCount();
			while (i < j)
			{
				mem = memList.GetItemNoCheck(i);
				ram = MemAllocNN(RAMInfo);
				if (UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->deviceLocator).SetTo(csptr))
				{
					ram->deviceLocator = Text::String::NewNotNullSlow(csptr);
				}
				else
				{
					ram->deviceLocator = 0;
				}
				if (UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->manufacturer).SetTo(csptr))
				{
					ram->manufacturer = Text::String::NewNotNullSlow(csptr);
				}
				else
				{
					ram->manufacturer = 0;
				}
				if (UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->partNo).SetTo(csptr))
				{
					ram->partNo = Text::String::NewNotNullSlow(csptr);
				}
				else
				{
					ram->partNo = 0;
				}
				if (UnsafeArrayOpt<const UTF8Char>::ConvertFrom(mem->sn).SetTo(csptr))
				{
					ram->sn = Text::String::NewNotNullSlow(csptr);
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

			smbios->FreeMemoryInfo(memList);
			smbios.Delete();
			return retCnt;
		}
		smbios.Delete();
	}


	NEW_CLASS(db, Win32::WMIQuery(L"ROOT\\CIMV2"));
//	r = db->ExecuteReader(L"select DeviceLocator, Manufacturer, PartNumber, SerialNumber, Speed from CIM_PhysicalMemory");
	if (db->ExecuteReaderW(L"select * from CIM_PhysicalMemory").SetTo(r))
	{
		Text::StringBuilderUTF8 sb;
		UIntOS devLocCol = (UIntOS)-1;
		UIntOS manuCol = (UIntOS)-1;
		UIntOS partNoCol = (UIntOS)-1;
		UIntOS snCol = (UIntOS)-1;
		UIntOS spdCol = (UIntOS)-1;
		UIntOS dataWCol = (UIntOS)-1;
		UIntOS totalWCol = (UIntOS)-1;
		UIntOS i;
		i = r->ColCount();
		while (i-- > 0)
		{
			if (r->GetName(i, sbuff).SetTo(sptr))
			{
				if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("DeviceLocator")))
				{
					devLocCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Manufacturer")))
				{
					manuCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("PartNumber")))
				{
					partNoCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("SerialNumber")))
				{
					snCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Speed")))
				{
					spdCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("DataWidth")))
				{
					dataWCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("TotalWidth")))
				{
					totalWCol = i;
				}
			}
		}

		if (devLocCol != (UIntOS)-1 && manuCol != (UIntOS)-1 && partNoCol != (UIntOS)-1 && snCol != (UIntOS)-1 && spdCol != (UIntOS)-1 && dataWCol != (UIntOS)-1 && totalWCol != (UIntOS)-1)
		{
			while (r->ReadNext())
			{
				ram = MemAllocNN(RAMInfo);
				ram->deviceLocator = r->GetNewStr(devLocCol);
				ram->manufacturer = r->GetNewStr(manuCol);
				ram->partNo = r->GetNewStr(partNoCol);
				ram->sn = r->GetNewStr(snCol);
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
