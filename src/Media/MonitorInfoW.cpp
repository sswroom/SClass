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

Media::MonitorInfo::MonitorInfo(void *hMonitor)
{
	WChar sbuff[512];
	WChar *sarr[3];
	UOSInt i;
	MONITORINFOEXW info;
	info.cbSize = sizeof(info);
	if (GetMonitorInfoW((HMONITOR)hMonitor, &info))
	{
#if defined(_WIN32_WCE)
		this->name = Text::StrToUTF8New(info.szDevice);
		this->isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
		this->left = info.rcMonitor.left;
		this->top = info.rcMonitor.top;
		this->right = info.rcMonitor.right;
		this->bottom = info.rcMonitor.bottom;
		this->desc = 0;
		this->monId = 0;
#else
		DISPLAY_DEVICEW dev;
		this->name = Text::StrToUTF8New(info.szDevice);
		this->isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
		this->left = info.rcMonitor.left;
		this->top = info.rcMonitor.top;
		this->right = info.rcMonitor.right;
		this->bottom = info.rcMonitor.bottom;
		dev.cb = sizeof(dev);
		if (EnumDisplayDevicesW(info.szDevice, 0, &dev, 0))
		{
			this->desc = Text::StrToUTF8New(dev.DeviceString);
			Text::StrConcat(sbuff, dev.DeviceID);
			i = Text::StrSplit(sarr, 3, sbuff, '\\');
			if (i == 3)
			{
				this->monId = Text::StrToUTF8New(sarr[1]);
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
	SDEL_TEXT(this->name);
	SDEL_TEXT(this->desc);
	SDEL_TEXT(this->monId);
}

const UTF8Char *Media::MonitorInfo::GetName()
{
	return this->name;
}

const UTF8Char *Media::MonitorInfo::GetDesc()
{
	return this->desc;
}

const UTF8Char *Media::MonitorInfo::GetMonitorID()
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
