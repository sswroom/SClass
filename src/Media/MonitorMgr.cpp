#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Registry.h"
#include "IO/SystemInfo.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "Media/MonitorInfo.h"
#include "Media/MonitorMgr.h"
#include "Sync/MutexUsage.h"
#include "Text/MyStringW.h"

Media::MonitorMgr::MonitorMgr()
{
}

Media::MonitorMgr::~MonitorMgr()
{
	UOSInt i;
	MonitorSetting *mon;
	i = this->monMap.GetCount();
	while (i-- > 0)
	{
		mon = this->monMap.GetItem(i);
		MemFree(mon);
	}
}

Double Media::MonitorMgr::GetMonitorHDPI(MonitorHandle *hMonitor)
{
	Media::MonitorInfo monInfo(hMonitor);
	Text::String *monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	MonitorSetting *mon;
	Double hdpi;
	Double ddpi;
	Sync::MutexUsage mutUsage(&this->monMut);
	mon = this->monMap.Get(monName);
	if (mon)
	{
		hdpi = mon->hdpi;
	}
	else
	{
		hdpi = 96.0;
		ddpi = 96.0;
		IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
		if (reg)
		{
			const WChar *wptr = Text::StrToWCharNew(monName->v);
			IO::Registry *reg2 = reg->OpenSubReg(wptr);
			Text::StrDelNew(wptr);
			if (reg2)
			{
				hdpi = reg2->GetValueI32(L"MonitorHDPI") * 0.1;
				ddpi = reg2->GetValueI32(L"MonitorDDPI") * 0.1;
				IO::Registry::CloseRegistry(reg2);

				if (hdpi == 0)
				{
					hdpi = 96.0;
				}
				if (ddpi == 0)
				{
					ddpi = 96.0;
				}
			}
			IO::Registry::CloseRegistry(reg);
		}
		else
		{
			Media::DDCReader reader(hMonitor);
			UOSInt size;
			UInt8 *edid = reader.GetEDID(&size);
			if (edid)
			{
				Media::EDID::EDIDInfo info;
				if (Media::EDID::Parse(edid, &info))
				{
					if (info.dispPhysicalW != 0)
					{
						hdpi = monInfo.GetPixelWidth() / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_CENTIMETER, Math::Unit::Distance::DU_INCH, info.dispPhysicalW);
					}
					
				}
			}

			if (hdpi <= 72.0) //TV
			{
				ddpi = hdpi;
			}
			else
			{
				IO::SystemInfo sysInfo;
				switch (sysInfo.GetChassisType())
				{
				case IO::SystemInfo::CT_HANDHELD:
				case IO::SystemInfo::CT_LAPTOP:
				case IO::SystemInfo::CT_TABLET:
				case IO::SystemInfo::CT_NOTEBOOK:
					ddpi = 144.0;
					break;
				case IO::SystemInfo::CT_UNKNOWN:
				case IO::SystemInfo::CT_DESKTOP:
				case IO::SystemInfo::CT_LPDESKTOP:
				case IO::SystemInfo::CT_PIZZABOX:
				case IO::SystemInfo::CT_MINITOWER:
				case IO::SystemInfo::CT_TOWER:
				case IO::SystemInfo::CT_PORTABLE:
				case IO::SystemInfo::CT_DOCKINGSTATION:
				case IO::SystemInfo::CT_AIO:
				case IO::SystemInfo::CT_SUBNOTEBOOk:
				case IO::SystemInfo::CT_SPACESAVING:
				case IO::SystemInfo::CT_LUNCHBOX:
				case IO::SystemInfo::CT_MAINSERVER:
				case IO::SystemInfo::CT_EXPANSION:
				case IO::SystemInfo::CT_SUBCHASSIS:
				case IO::SystemInfo::CT_BUSEXPANSION:
				case IO::SystemInfo::CT_PERIPHERAL:
				case IO::SystemInfo::CT_RAID:
				case IO::SystemInfo::CT_RACKMOUNT:
				case IO::SystemInfo::CT_SEALED_CASE_PC:
				case IO::SystemInfo::CT_MULTISYSTEM:
				case IO::SystemInfo::CT_COMPACT_PCI:
				case IO::SystemInfo::CT_ADVANCEDTCA:
				case IO::SystemInfo::CT_BLADE:
				case IO::SystemInfo::CT_BLADEENCLOSURE:
				case IO::SystemInfo::CT_CONVERTIBLE:
				case IO::SystemInfo::CT_DETACHABLE:
				case IO::SystemInfo::CT_IOT_GATEWAY:
				case IO::SystemInfo::CT_EMBEDDEDPC:
				case IO::SystemInfo::CT_MINI_PC:
				case IO::SystemInfo::CT_STICK_PC:
				default:
					ddpi = 96.0;
					break;
				}
			}
		}

		mon = MemAlloc(MonitorSetting, 1);
		mon->hdpi = hdpi;
		mon->ddpi = ddpi;
		this->monMap.Put(monName, mon);
	}
	mutUsage.EndUse();
	return hdpi;
}

void Media::MonitorMgr::SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI)
{
	Media::MonitorInfo monInfo(hMonitor);
	Text::String *monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	Text::String *monDesc = monInfo.GetDesc();
	MonitorSetting *mon;
	Sync::MutexUsage mutUsage(&this->monMut);
	mon = this->monMap.Get(monName);
	if (mon)
	{
		mon->hdpi = monitorHDPI;
	}
	else
	{
		mon = MemAlloc(MonitorSetting, 1);
		mon->hdpi = monitorHDPI;
		mon->ddpi = 96.0;
		this->monMap.Put(monName, mon);
	}
	mutUsage.EndUse();

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
	if (reg)
	{
		const WChar *wptr = Text::StrToWCharNew(monName->v);
		IO::Registry *reg2 = reg->OpenSubReg(wptr);
		Text::StrDelNew(wptr);
		if (reg2)
		{
			reg2->SetValue(L"MonitorHDPI", Double2Int32(monitorHDPI * 10));
			if (monDesc)
			{
				wptr = Text::StrToWCharNew(monDesc->v);
				reg2->SetValue(L"Desc", wptr);
				Text::StrDelNew(wptr);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		IO::Registry::CloseRegistry(reg);
	}
}

Double Media::MonitorMgr::GetMonitorDDPI(MonitorHandle *hMonitor)
{
	Media::MonitorInfo monInfo(hMonitor);
	Text::String *monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	MonitorSetting *mon;
	Double ddpi;
	Sync::MutexUsage mutUsage(&this->monMut);
	mon = this->monMap.Get(monName);
	if (mon)
	{
		ddpi = mon->ddpi;
		mutUsage.EndUse();
	}
	else
	{
		mutUsage.EndUse();
		GetMonitorHDPI(hMonitor);
		mutUsage.BeginUse();
		mon = this->monMap.Get(monName);
		if (mon)
		{
			ddpi = mon->ddpi;
			mutUsage.EndUse();
		}
		else
		{
			mutUsage.EndUse();
			ddpi = 96.0;
		}
	}
	return ddpi;
}

void Media::MonitorMgr::SetMonitorDDPI(MonitorHandle *hMonitor, Double monitorDDPI)
{
	Media::MonitorInfo monInfo(hMonitor);
	Text::String *monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	Text::String *monDesc = monInfo.GetDesc();
	MonitorSetting *mon;
	Sync::MutexUsage mutUsage(&this->monMut);
	mon = this->monMap.Get(monName);
	if (mon)
	{
		mon->ddpi = monitorDDPI;
	}
	else
	{
		mon = MemAlloc(MonitorSetting, 1);
		mon->hdpi = 96.0;
		mon->ddpi = monitorDDPI;
		this->monMap.Put(monName, mon);
	}
	mutUsage.EndUse();

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
	if (reg)
	{
		const WChar *wptr = Text::StrToWCharNew(monName->v);
		IO::Registry *reg2 = reg->OpenSubReg(wptr);
		Text::StrDelNew(wptr);
		if (reg2)
		{
			reg2->SetValue(L"MonitorDDPI", Double2Int32(monitorDDPI * 10));
			if (monDesc)
			{
				wptr = Text::StrToWCharNew(monDesc->v);
				reg2->SetValue(L"Desc", wptr);
				Text::StrDelNew(wptr);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		IO::Registry::CloseRegistry(reg);
	}
}
