#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "Media/MonitorInfo.h"
#include "Text/MyString.h"
#include <gtk/gtk.h>

Media::MonitorInfo::MonitorInfo(void *hMonitor)
{
	OSInt monNum = ((OSInt)hMonitor) - 1;
	GdkRectangle rect;
	UTF8Char sbuff[32];
//#define GDK_MAJOR_VERSION (3)
//#define GDK_MINOR_VERSION (22)
//#define GDK_MICRO_VERSION (11)		
#if GDK_MAJOR_VERSION > 3 || (GDK_MAJOR_VERSION == 3 && GDK_MINOR_VERSION >= 22)
	GdkDisplay *display = gdk_display_get_default();
	GdkMonitor *mon;
	if (hMonitor == 0)
	{
		if (display)
		{
			int i = 0;
			int j = gdk_display_get_n_monitors(display);
			while (i < j)
			{
				mon = gdk_display_get_monitor(display, i);
				if (gdk_monitor_is_primary(mon))
				{
					monNum = i;
					break;
				}
				i++;
			}

		}
	}
	if (display && (mon = gdk_display_get_monitor(display, monNum)) != 0)
	{
		this->isPrimary = gdk_monitor_is_primary(mon);
		gdk_monitor_get_geometry(mon, &rect);
		this->left = rect.x;
		this->top = rect.y;
		this->right = rect.width + rect.x;
		this->bottom = rect.height + rect.y;
	}
#else
	GdkScreen *scn = gdk_screen_get_default();
	if (hMonitor == 0)
	{
		monNum = gdk_screen_get_primary_monitor(scn); 
	}
	if (scn)
	{
		this->isPrimary = (monNum == gdk_screen_get_primary_monitor(scn));
		gdk_screen_get_monitor_geometry(scn, monNum, &rect);
		this->left = rect.x;
		this->top = rect.y;
		this->right = rect.width + rect.x;
		this->bottom = rect.height + rect.y;
	}
#endif
	else
	{
		this->isPrimary = true;
		this->left = 0;
		this->top = 0;
		this->right = 320;
		this->bottom = 240;
	}

	UOSInt size;
	UInt8 *edid;
	Media::DDCReader reader(hMonitor);
	edid = reader.GetEDID(&size);
	if (edid)
	{
		Media::EDID::EDIDInfo info;
		if (Media::EDID::Parse(edid, &info))
		{
			this->name = Text::StrCopyNew(info.monitorName);
			Text::StrHexVal16(Text::StrConcat(sbuff, info.vendorName), info.productCode);
			this->monId = Text::StrCopyNew(sbuff);
		}
		else
		{
			this->name = Text::StrCopyNew((const UTF8Char*)"Monitor");
			this->monId = 0;
		}
		this->desc = 0;
	}
	else
	{
		this->name = Text::StrCopyNew((const UTF8Char*)"Monitor");
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

