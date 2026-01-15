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

Media::MonitorInfo::MonitorInfo(Optional<MonitorHandle> hMonitor)
{
	WChar wbuff[512];
	UnsafeArray<WChar> sarr[3];
	UIntOS i;
	MONITORINFOEXW info;
	info.cbSize = sizeof(info);
	if (GetMonitorInfoW((HMONITOR)hMonitor.OrNull(), &info))
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
				this->monId = Text::String::NewNotNull(UnsafeArray<const WChar>(sarr[1]));
			}
			else
			{
				this->monId = nullptr;
			}
		}
		else
		{
			this->desc = nullptr;
			this->monId = nullptr;
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
		this->name = Text::String::NewEmpty();
		this->desc = nullptr;
		this->monId = nullptr;
	}
}

Media::MonitorInfo::~MonitorInfo()
{
	this->name->Release();
	OPTSTR_DEL(this->desc);
	OPTSTR_DEL(this->monId);
}

NN<Text::String> Media::MonitorInfo::GetName() const
{
	return this->name;
}

Optional<Text::String> Media::MonitorInfo::GetDesc() const
{
	return this->desc;
}

Optional<Text::String> Media::MonitorInfo::GetMonitorID() const
{
	return this->monId;
}

Int32 Media::MonitorInfo::GetLeft() const
{
	return this->left;
}

Int32 Media::MonitorInfo::GetTop() const
{
	return this->top;
}

Int32 Media::MonitorInfo::GetPixelWidth() const
{
	return this->right - this->left;
}

Int32 Media::MonitorInfo::GetPixelHeight() const
{
	return this->bottom - this->top;
}

Bool Media::MonitorInfo::IsPrimary()
{
	return this->isPrimary;
}
