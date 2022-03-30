#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/MonitorInfo.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#ifdef _WIN32_WCE
#define MONITORINFOEXW MONITORINFOEX
#define GetMonitorInfoW GetMonitorInfo
#endif

Media::MonitorInfo::MonitorInfo(MonitorHandle *hMonitor)
{
	WChar wbuff[512];
	WChar *sarr[3];
	UOSInt i;
	MONITORINFOEXW info;
	info.cbSize = sizeof(info);
	if (GetMonitorInfoW((HMONITOR)hMonitor, &info))
	{
#if defined(_WIN32_WCE)
		this->name = Text::String::NewNotNull(info.szDevice);
		this->isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
		this->left = info.rcMonitor.left;
		this->top = info.rcMonitor.top;
		this->right = info.rcMonitor.right;
		this->bottom = info.rcMonitor.bottom;
		this->desc = 0;
		this->monId = 0;
#else
		DISPLAY_DEVICEW dev;
		this->name = Text::String::NewNotNull(info.szDevice);
		this->isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
		this->left = info.rcMonitor.left;
		this->top = info.rcMonitor.top;
		this->right = info.rcMonitor.right;
		this->bottom = info.rcMonitor.bottom;
		dev.cb = sizeof(dev);
		if (EnumDisplayDevicesW(info.szDevice, 0, &dev, 0))
		{
			this->desc = Text::String::NewNotNull(dev.DeviceString);
			Text::StrConcat(wbuff, dev.DeviceID);
			i = Text::StrSplit(sarr, 3, wbuff, '\\');
			if (i == 3)
			{
				this->monId = Text::String::NewNotNull(sarr[1]);
			}
			else
			{
				this->monId = 0;
			}
		}
		else
		{
			this->desc = 0;
			this->monId = 0;
		}
#endif
	}
	else
	{
		this->left = 0;
		this->top = 0;
		this->right = 0;
		this->bottom = 0;
		this->isPrimary = false;
		this->name = 0;
		this->desc = 0;
		this->monId = 0;
	}
}

Media::MonitorInfo::~MonitorInfo()
{
	SDEL_STRING(this->name);
	SDEL_STRING(this->desc);
	SDEL_STRING(this->monId);
}

Text::String *Media::MonitorInfo::GetName()
{
	return this->name;
}

Text::String *Media::MonitorInfo::GetDesc()
{
	return this->desc;
}

Text::String *Media::MonitorInfo::GetMonitorID()
{
	return this->monId;
}

Int32 Media::MonitorInfo::GetLeft()
{
	return this->left;
}

Int32 Media::MonitorInfo::GetTop()
{
	return this->top;
}

Int32 Media::MonitorInfo::GetPixelWidth()
{
	return this->right - this->left;
}

Int32 Media::MonitorInfo::GetPixelHeight()
{
	return this->bottom - this->top;
}

Bool Media::MonitorInfo::IsPrimary()
{
	return this->isPrimary;
}
