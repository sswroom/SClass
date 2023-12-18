#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/GTKDrawEngine.h"
#include "UI/GUICore.h"
#include "UI/GUICoreGTK.h"
#include "UI/GTK/GTKButton.h"
#include "UI/GTK/GTKComboBox.h"
#include "UI/GTK/GTKGroupBox.h"
#include "UI/GTK/GTKHSplitter.h"
#include "UI/GTK/GTKLabel.h"
#include "UI/GTK/GTKMessageDialog.h"
#include "UI/GTK/GTKVSplitter.h"
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#undef Bool

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

NotNullPtr<Media::DrawEngine> UI::GUICoreGTK::CreateDrawEngine()
{
	NotNullPtr<Media::DrawEngine> deng;
	NEW_CLASSNN(deng, Media::GTKDrawEngine());
	return deng;
};

Double UI::GUICoreGTK::GetMagnifyRatio(MonitorHandle *hMonitor)
{
	Double v = gdk_screen_get_resolution(gdk_screen_get_default()) / 96.0;
	if (v <= 0)
		v = 1.0;
	return v;
}

void UI::GUICoreGTK::UseDevice(Bool useSystem, Bool useDisplay)
{
	Display *display = XOpenDisplay(0);
	XResetScreenSaver(display);
	XCloseDisplay(display);
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

Math::Size2D<UOSInt> UI::GUICoreGTK::GetDesktopSize()
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
	return Math::Size2D<UOSInt>((UOSInt)(maxX - minX), (UOSInt)(maxY - minY));
#else
	GdkScreen *scn = gdk_screen_get_default();
	return Math::Size2D<UOSInt>(gdk_screen_get_width(scn), gdk_screen_get_height(scn));
#endif
}

Math::Coord2D<OSInt> UI::GUICoreGTK::GetCursorPos()
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
	return Math::Coord2D<OSInt>(outx, outy);
#else
	GdkDeviceManager *devMgr = gdk_display_get_device_manager(display);
	GdkDevice *dev = gdk_device_manager_get_client_pointer(devMgr);
	gint outx;
	gint outy;
	gdk_device_get_position(dev, 0, &outx, &outy);
	return Math::Coord2D<OSInt>(outx, outy);
#endif
}

void UI::GUICoreGTK::SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot)
{
}

void UI::GUICoreGTK::GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi)
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

Media::MonitorMgr *UI::GUICoreGTK::GetMonitorMgr()
{
	return this->monMgr;
}

Bool UI::GUICoreGTK::IsForwarded()
{
	return getenv("SSH_CLIENT") != 0;
}

void UI::GUICoreGTK::ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	UI::GTK::GTKMessageDialog::ShowOK(message, title, ctrl);
}

Bool UI::GUICoreGTK::ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return UI::GTK::GTKMessageDialog::ShowYesNo(message, title, ctrl);
}

NotNullPtr<UI::GUIButton> UI::GUICoreGTK::NewButton(NotNullPtr<GUIClientControl> parent, Text::CStringNN text)
{
	NotNullPtr<UI::GTK::GTKButton> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKButton(*this, parent, text));
	return ctrl;
}

NotNullPtr<UI::GUIComboBox> UI::GUICoreGTK::NewComboBox(NotNullPtr<GUIClientControl> parent, Bool allowEdit)
{
	NotNullPtr<UI::GTK::GTKComboBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKComboBox(*this, parent, allowEdit));
	return ctrl;
}

NotNullPtr<UI::GUIGroupBox> UI::GUICoreGTK::NewGroupBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN text)
{
	NotNullPtr<UI::GTK::GTKGroupBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKGroupBox(*this, parent, text));
	return ctrl;
}

NotNullPtr<UI::GUILabel> UI::GUICoreGTK::NewLabel(NotNullPtr<GUIClientControl> parent, Text::CStringNN text)
{
	NotNullPtr<UI::GTK::GTKLabel> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKLabel(*this, parent, text));
	return ctrl;
}

NotNullPtr<UI::GUIHSplitter> UI::GUICoreGTK::NewHSplitter(NotNullPtr<GUIClientControl> parent, Int32 width, Bool isRight)
{
	NotNullPtr<UI::GTK::GTKHSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKHSplitter(*this, parent, width, isRight));
	return ctrl;
}

NotNullPtr<UI::GUIVSplitter> UI::GUICoreGTK::NewVSplitter(NotNullPtr<GUIClientControl> parent, Int32 height, Bool isBottom)
{
	NotNullPtr<UI::GTK::GTKVSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKVSplitter(*this, parent, height, isBottom));
	return ctrl;
}
