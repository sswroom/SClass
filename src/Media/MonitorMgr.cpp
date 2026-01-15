#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Registry.h"
#include "IO/SystemInfo.h"
#include "Math/Math_C.h"
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
	UIntOS i;
	NN<MonitorSetting> mon;
	i = this->monMap.GetCount();
	while (i-- > 0)
	{
		mon = this->monMap.GetItemNoCheck(i);
		MemFreeNN(mon);
	}
}

Double Media::MonitorMgr::GetMonitorHDPI(Optional<MonitorHandle> hMonitor)
{
	Media::MonitorInfo monInfo(hMonitor);
	NN<Text::String> monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	NN<MonitorSetting> mon;
	Double hdpi;
	Double ddpi;
	Sync::MutexUsage mutUsage(this->monMut);
	if (this->monMap.GetNN(monName).SetTo(mon))
	{
		hdpi = mon->hdpi;
	}
	else
	{
		hdpi = 96.0;
		ddpi = 96.0;
		NN<IO::Registry> reg;
		if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
		{
			UnsafeArray<const WChar> wptr = Text::StrToWCharNew(monName->v);
			NN<IO::Registry> reg2;
			if (reg->OpenSubReg(wptr).SetTo(reg2))
			{
				Text::StrDelNew(wptr);
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
			else
			{
				Text::StrDelNew(wptr);
			}
			IO::Registry::CloseRegistry(reg);
		}
		else
		{
			Media::DDCReader reader(hMonitor);
			UIntOS size;
			UnsafeArray<UInt8> edid;
			if (reader.GetEDID(size).SetTo(edid))
			{
				Media::EDID::EDIDInfo info;
				if (Media::EDID::Parse(edid, info))
				{
					if (info.dispPhysicalW_mm != 0)
					{
						hdpi = monInfo.GetPixelWidth() / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, info.dispPhysicalW_mm);
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

		mon = MemAllocNN(MonitorSetting);
		mon->hdpi = hdpi;
		mon->ddpi = ddpi;
		this->monMap.PutNN(monName, mon);
	}
	mutUsage.EndUse();
	return hdpi;
}

void Media::MonitorMgr::SetMonitorHDPI(Optional<MonitorHandle> hMonitor, Double monitorHDPI)
{
	Media::MonitorInfo monInfo(hMonitor);
	NN<Text::String> monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	Optional<Text::String> monDesc = monInfo.GetDesc();
	NN<Text::String> nnmonDesc;
	NN<MonitorSetting> mon;
	Sync::MutexUsage mutUsage(this->monMut);
	if (this->monMap.GetNN(monName).SetTo(mon))
	{
		mon->hdpi = monitorHDPI;
	}
	else
	{
		mon = MemAllocNN(MonitorSetting);
		mon->hdpi = monitorHDPI;
		mon->ddpi = 96.0;
		this->monMap.PutNN(monName, mon);
	}
	mutUsage.EndUse();

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		UnsafeArray<const WChar> wptr = Text::StrToWCharNew(monName->v);
		NN<IO::Registry> reg2;
		if (reg->OpenSubReg(wptr).SetTo(reg2))
		{
			Text::StrDelNew(wptr);
			reg2->SetValue(L"MonitorHDPI", Double2Int32(monitorHDPI * 10));
			if (monDesc.SetTo(nnmonDesc))
			{
				wptr = Text::StrToWCharNew(nnmonDesc->v);
				reg2->SetValue(L"Desc", wptr);
				Text::StrDelNew(wptr);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		else
		{
			Text::StrDelNew(wptr);
		}
		IO::Registry::CloseRegistry(reg);
	}
}

Double Media::MonitorMgr::GetMonitorDDPI(Optional<MonitorHandle> hMonitor)
{
	Media::MonitorInfo monInfo(hMonitor);
	NN<Text::String> monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	NN<MonitorSetting> mon;
	Double ddpi;
	Sync::MutexUsage mutUsage(this->monMut);
	if (this->monMap.GetNN(monName).SetTo(mon))
	{
		ddpi = mon->ddpi;
		mutUsage.EndUse();
	}
	else
	{
		mutUsage.EndUse();
		GetMonitorHDPI(hMonitor);
		mutUsage.BeginUse();
		if (this->monMap.GetNN(monName).SetTo(mon))
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

void Media::MonitorMgr::SetMonitorDDPI(Optional<MonitorHandle> hMonitor, Double monitorDDPI)
{
	Media::MonitorInfo monInfo(hMonitor);
	NN<Text::String> monName = Text::String::OrEmpty(monInfo.GetMonitorID());
	Optional<Text::String> monDesc = monInfo.GetDesc();
	NN<Text::String> nnmonDesc;
	NN<MonitorSetting> mon;
	Sync::MutexUsage mutUsage(this->monMut);
	if (this->monMap.GetNN(monName).SetTo(mon))
	{
		mon->ddpi = monitorDDPI;
	}
	else
	{
		mon = MemAllocNN(MonitorSetting);
		mon->hdpi = 96.0;
		mon->ddpi = monitorDDPI;
		this->monMap.PutNN(monName, mon);
	}
	mutUsage.EndUse();

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		UnsafeArray<const WChar> wptr = Text::StrToWCharNew(monName->v);
		NN<IO::Registry> reg2;
		if (reg->OpenSubReg(wptr).SetTo(reg2))
		{
			Text::StrDelNew(wptr);
			reg2->SetValue(L"MonitorDDPI", Double2Int32(monitorDDPI * 10));
			if (monDesc.SetTo(nnmonDesc))
			{
				wptr = Text::StrToWCharNew(nnmonDesc->v);
				reg2->SetValue(L"Desc", wptr);
				Text::StrDelNew(wptr);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		else
		{
			Text::StrDelNew(wptr);
		}
		IO::Registry::CloseRegistry(reg);
	}
}
