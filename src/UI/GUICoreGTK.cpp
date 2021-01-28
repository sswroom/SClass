#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/GTKDrawEngine.h"
#include "UI/GUICore.h"
#include "UI/GUICoreGTK.h"
#include <gtk/gtk.h>

UI::GUICoreGTK::GUICoreGTK()
{
	this->monMgr = 0;
	GdkScreen *screen = gdk_screen_get_default();
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(provider, "combobox, button, entry {min-height: 0px; min-width: 0px; padding: 0px; margin: 0px}", -1, 0);
	gtk_style_context_add_provider_for_screen(screen, (GtkStyleProvider*)provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

UI::GUICoreGTK::~GUICoreGTK()
{
}

void UI::GUICoreGTK::Run()
{
	gtk_main();
}

void UI::GUICoreGTK::ProcessMessages()
{
	while (gtk_events_pending())
	{
		gtk_main_iteration();
	}
}

void UI::GUICoreGTK::WaitForMessages()
{
}

void UI::GUICoreGTK::Exit()
{
	gtk_main_quit();
}

Media::DrawEngine *UI::GUICoreGTK::CreateDrawEngine()
{
	Media::DrawEngine *deng;
	NEW_CLASS(deng, Media::GTKDrawEngine());
	return deng;
};

Double UI::GUICoreGTK::GetMagnifyRatio(void *hMonitor)
{
	Double v = gdk_screen_get_resolution(gdk_screen_get_default()) / 96.0;
	if (v <= 0)
		v = 1.0;
	return v;
}

void UI::GUICoreGTK::UseDevice(Bool useSystem, Bool useDisplay)
{
}

void UI::GUICoreGTK::SetNoDisplayOff(Bool noDispOff)
{
}

void UI::GUICoreGTK::DisplayOff()
{
}

void UI::GUICoreGTK::Suspend()
{
}

void UI::GUICoreGTK::GetDesktopSize(UOSInt *w, UOSInt *h)
{
#if GDK_MAJOR_VERSION > 3 || (GDK_MAJOR_VERSION == 3 && GDK_MINOR_VERSION >= 22)
	GdkDisplay *display = gdk_display_get_default();
	GdkRectangle rect;
	GdkMonitor *mon;
	int minX = 0;
	int minY = 0;
	int maxX = 0;
	int maxY = 0;
	int i;
	int j = gdk_display_get_n_monitors(display);
	i = 0;
	while (i < j)
	{
		mon = gdk_display_get_monitor(display, i);
		gdk_monitor_get_workarea(mon, &rect);
		if (minX > rect.x)
		{
			minX = rect.x;
		}
		if (minY > rect.y)
		{
			minY = rect.y;
		}
		if (maxX < rect.x + rect.width)
		{
			maxX = rect.x + rect.width;
		}
		if (maxY < rect.y + rect.height)
		{
			maxY = rect.y + rect.height;
		}
		i++;
	}
	*w = maxX - minX;
	*h = maxY - minY;
#else
	GdkScreen *scn = gdk_screen_get_default();
	*w = gdk_screen_get_width(scn);
	*h = gdk_screen_get_height(scn);
#endif
}

void UI::GUICoreGTK::GetCursorPos(OSInt *x, OSInt *y)
{
	GdkDisplay *display = gdk_display_get_default();
#if GDK_MAJOR_VERSION > 3 || (GDK_MAJOR_VERSION == 3 && GDK_MINOR_VERSION >= 22)
	GdkScreen *scn;
	GdkSeat *seat = gdk_display_get_default_seat(display);
	GdkDevice *dev = gdk_seat_get_pointer(seat);
	gint outx = 0;
	gint outy = 0;
	if (dev)
	{
		gdk_device_get_position(dev, &scn, &outx, &outy);
	}
	*x = outx;
	*y = outy;
#else
	GdkDeviceManager *devMgr = gdk_display_get_device_manager(display);
	GdkDevice *dev = gdk_device_manager_get_client_pointer(devMgr);
	gint outx;
	gint outy;
	gdk_device_get_position(dev, 0, &outx, &outy);
	*x = outx;
	*y = outy;
#endif
}

void UI::GUICoreGTK::SetDisplayRotate(void *hMonitor, DisplayRotation rot)
{
}

void UI::GUICoreGTK::GetMonitorDPIs(void *hMonitor, Double *hdpi, Double *ddpi)
{
	if (this->monMgr)
	{
		if (hdpi)
			*hdpi = this->monMgr->GetMonitorHDPI(hMonitor);
		if (ddpi)
			*ddpi = this->monMgr->GetMonitorDDPI(hMonitor);
	}
	else
	{
		if (hdpi)
			*hdpi = 96.0;
		if (ddpi)
			*ddpi = 96.0;
	}
}

void UI::GUICoreGTK::SetMonitorMgr(Media::MonitorMgr *monMgr)
{
	this->monMgr = monMgr;
}

