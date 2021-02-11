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
	NEW_CLASS(this->monMut, Sync::Mutex());
	NEW_CLASS(this->monMap, Data::StringUTF8Map<MonitorSetting*>());
}

Media::MonitorMgr::~MonitorMgr()
{
	OSInt i;
	Data::ArrayList<MonitorSetting*> *monList = this->monMap->GetValues();
	MonitorSetting *mon;
	i = monList->GetCount();
	while (i-- > 0)
	{
		mon = monList->GetItem(i);
		MemFree(mon);
	}
	DEL_CLASS(this->monMut);
	DEL_CLASS(this->monMap);
}

Double Media::MonitorMgr::GetMonitorHDPI(void *hMonitor)
{
	Media::MonitorInfo monInfo(hMonitor);
	const UTF8Char *monName = monInfo.GetMonitorID();
	if (monName == 0)
	{
		monName = (const UTF8Char*)"";
	}
	MonitorSetting *mon;
	Double hdpi;
	Double ddpi;
	Sync::MutexUsage mutUsage(this->monMut);
	mon = this->monMap->Get(monName);
	if (mon)
	{
		hdpi = mon->hdpi;
	}
	else
	{
		hdpi = 96.0;
		ddpi = 96.0;
		IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"sswr", L"AVIRead");
		if (reg)
		{
			const WChar *wptr = Text::StrToWCharNew(monName);
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
				default:
					ddpi = 96.0;
					break;
				}
			}
		}

		mon = MemAlloc(MonitorSetting, 1);
		mon->hdpi = hdpi;
		mon->ddpi = ddpi;
		this->monMap->Put(monName, mon);
	}
	mutUsage.EndUse();
	return hdpi;
}

void Media::MonitorMgr::SetMonitorHDPI(void *hMonitor, Double monitorHDPI)
{
	Media::MonitorInfo monInfo(hMonitor);
	const UTF8Char *monName = monInfo.GetMonitorID();
	const UTF8Char *monDesc = monInfo.GetDesc();
	MonitorSetting *mon;
	Sync::MutexUsage mutUsage(this->monMut);
	mon = this->monMap->Get(monName);
	if (mon)
	{
		mon->hdpi = monitorHDPI;
	}
	else
	{
		mon = MemAlloc(MonitorSetting, 1);
		mon->hdpi = monitorHDPI;
		mon->ddpi = 96.0;
		this->monMap->Put(monName, mon);
	}
	mutUsage.EndUse();

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"sswr", L"AVIRead");
	if (reg)
	{
		const WChar *wptr = Text::StrToWCharNew(monName);
		IO::Registry *reg2 = reg->OpenSubReg(wptr);
		Text::StrDelNew(wptr);
		if (reg2)
		{
			reg2->SetValue(L"MonitorHDPI", Math::Double2Int32(monitorHDPI * 10));
			if (monDesc)
			{
				wptr = Text::StrToWCharNew(monDesc);
				reg2->SetValue(L"Desc", wptr);
				Text::StrDelNew(wptr);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		IO::Registry::CloseRegistry(reg);
	}
}

Double Media::MonitorMgr::GetMonitorDDPI(void *hMonitor)
{
	Media::MonitorInfo monInfo(hMonitor);
	const UTF8Char *monName = monInfo.GetMonitorID();
	if (monName == 0)
	{
		monName = (const UTF8Char*)"";
	}
	MonitorSetting *mon;
	Double ddpi;
	Sync::MutexUsage mutUsage(this->monMut);
	mon = this->monMap->Get(monName);
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
		mon = this->monMap->Get(monName);
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

void Media::MonitorMgr::SetMonitorDDPI(void *hMonitor, Double monitorDDPI)
{
	Media::MonitorInfo monInfo(hMonitor);
	const UTF8Char *monName = monInfo.GetMonitorID();
	const UTF8Char *monDesc = monInfo.GetDesc();
	MonitorSetting *mon;
	Sync::MutexUsage mutUsage(this->monMut);
	mon = this->monMap->Get(monName);
	if (mon)
	{
		mon->ddpi = monitorDDPI;
	}
	else
	{
		mon = MemAlloc(MonitorSetting, 1);
		mon->hdpi = 96.0;
		mon->ddpi = monitorDDPI;
		this->monMap->Put(monName, mon);
	}
	mutUsage.EndUse();

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"sswr", L"AVIRead");
	if (reg)
	{
		const WChar *wptr = Text::StrToWCharNew(monName);
		IO::Registry *reg2 = reg->OpenSubReg(wptr);
		Text::StrDelNew(wptr);
		if (reg2)
		{
			reg2->SetValue(L"MonitorDDPI", Math::Double2Int32(monitorDDPI * 10));
			if (monDesc)
			{
				wptr = Text::StrToWCharNew(monDesc);
				reg2->SetValue(L"Desc", wptr);
				Text::StrDelNew(wptr);
			}
			IO::Registry::CloseRegistry(reg2);
		}
		IO::Registry::CloseRegistry(reg);
	}
}
