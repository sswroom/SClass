#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "Media/MonitorInfo.h"
#include "Text/MyString.h"
#include <gtk/gtk.h>

Media::MonitorInfo::MonitorInfo(Optional<MonitorHandle> hMonitor)
{
	OSInt monNum = ((OSInt)hMonitor.OrNull()) - 1;
	GdkRectangle rect;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
#if GDK_MAJOR_VERSION > 3 || (GDK_MAJOR_VERSION == 3 && GDK_MINOR_VERSION >= 22)
	GdkDisplay *display = gdk_display_get_default();
	GdkMonitor *mon;
	if (hMonitor.IsNull())
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
	if (display && (mon = gdk_display_get_monitor(display, (int)monNum)) != 0)
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
	if (hMonitor.IsNull())
	{
		monNum = gdk_screen_get_primary_monitor(scn); 
	}
	if (scn)
	{
		this->isPrimary = (monNum == gdk_screen_get_primary_monitor(scn));
		gdk_screen_get_monitor_geometry(scn, (gint)monNum, &rect);
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
	UnsafeArray<UInt8> edid;
	Media::DDCReader reader(hMonitor);
	if (reader.GetEDID(size).SetTo(edid))
	{
		Media::EDID::EDIDInfo info;
		if (Media::EDID::Parse(edid, info))
		{
			this->name = Text::String::NewNotNullSlow(info.monitorName);
			sptr = Text::StrHexVal16(Text::StrConcat(sbuff, info.vendorName), info.productCode);
			this->monId = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		else
		{
			this->name = Text::String::New(UTF8STRC("Monitor"));
			this->monId = 0;
		}
		this->desc = 0;
	}
	else
	{
		this->name = Text::String::New(UTF8STRC("Monitor"));
		this->desc = 0;
		this->monId = 0;
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

