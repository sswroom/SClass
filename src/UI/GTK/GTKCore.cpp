#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/GTKDrawEngine.h"
#include "UI/GUICore.h"
#include "UI/GTK/GTKButton.h"
#include "UI/GTK/GTKCheckBox.h"
#include "UI/GTK/GTKCheckedListBox.h"
#include "UI/GTK/GTKComboBox.h"
#include "UI/GTK/GTKDateTimePicker.h"
#include "UI/GTK/GTKCore.h"
#include "UI/GTK/GTKFileDialog.h"
#include "UI/GTK/GTKFolderDialog.h"
#include "UI/GTK/GTKFontDialog.h"
#include "UI/GTK/GTKGroupBox.h"
#include "UI/GTK/GTKHScrollBar.h"
#include "UI/GTK/GTKHSplitter.h"
#include "UI/GTK/GTKLabel.h"
#include "UI/GTK/GTKListBox.h"
#include "UI/GTK/GTKMessageDialog.h"
#include "UI/GTK/GTKPanelBase.h"
#include "UI/GTK/GTKPictureBox.h"
#include "UI/GTK/GTKPictureBoxSimple.h"
#include "UI/GTK/GTKProgressBar.h"
#include "UI/GTK/GTKRadioButton.h"
#include "UI/GTK/GTKRealtimeLineChart.h"
#include "UI/GTK/GTKTabControl.h"
#include "UI/GTK/GTKTextBox.h"
#include "UI/GTK/GTKTrackBar.h"
#include "UI/GTK/GTKVSplitter.h"
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#undef Bool

UI::GTK::GTKCore::GTKCore()
{
	this->monMgr = 0;
	GdkScreen *screen = gdk_screen_get_default();
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(provider, "combobox, button, entry {min-height: 0px; min-width: 0px; padding: 0px; margin: 0px}", -1, 0);
	gtk_style_context_add_provider_for_screen(screen, (GtkStyleProvider*)provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

UI::GTK::GTKCore::~GTKCore()
{
}

void UI::GTK::GTKCore::Run()
{
	gtk_main();
}

void UI::GTK::GTKCore::ProcessMessages()
{
	while (gtk_events_pending())
	{
		gtk_main_iteration();
	}
}

void UI::GTK::GTKCore::WaitForMessages()
{
}

void UI::GTK::GTKCore::Exit()
{
	gtk_main_quit();
}

NN<Media::DrawEngine> UI::GTK::GTKCore::CreateDrawEngine()
{
	NN<Media::DrawEngine> deng;
	NEW_CLASSNN(deng, Media::GTKDrawEngine());
	return deng;
};

Double UI::GTK::GTKCore::GetMagnifyRatio(Optional<MonitorHandle> hMonitor)
{
	Double v = gdk_screen_get_resolution(gdk_screen_get_default()) / 96.0;
	if (v <= 0)
		v = 1.0;
	return v;
}

void UI::GTK::GTKCore::UseDevice(Bool useSystem, Bool useDisplay)
{
	Display *display = XOpenDisplay(0);
	XResetScreenSaver(display);
	XCloseDisplay(display);
}

void UI::GTK::GTKCore::SetNoDisplayOff(Bool noDispOff)
{
}

void UI::GTK::GTKCore::DisplayOff()
{
}

void UI::GTK::GTKCore::Suspend()
{
}

Math::Size2D<UOSInt> UI::GTK::GTKCore::GetDesktopSize()
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

Math::Coord2D<OSInt> UI::GTK::GTKCore::GetCursorPos()
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

void UI::GTK::GTKCore::SetDisplayRotate(Optional<MonitorHandle> hMonitor, DisplayRotation rot)
{
}

void UI::GTK::GTKCore::GetMonitorDPIs(Optional<MonitorHandle> hMonitor, OutParam<Double> hdpi, OutParam<Double> ddpi)
{
	NN<Media::MonitorMgr> monMgr;
	if (this->monMgr.SetTo(monMgr))
	{
		hdpi.Set(monMgr->GetMonitorHDPI(hMonitor));
		ddpi.Set(monMgr->GetMonitorDDPI(hMonitor));
	}
	else
	{
		hdpi.Set(96.0);
		ddpi.Set(96.0);
	}
}

void UI::GTK::GTKCore::SetMonitorMgr(Optional<Media::MonitorMgr> monMgr)
{
	this->monMgr = monMgr;
}

Optional<Media::MonitorMgr> UI::GTK::GTKCore::GetMonitorMgr()
{
	return this->monMgr;
}

Bool UI::GTK::GTKCore::IsForwarded()
{
	return getenv("SSH_CLIENT") != 0;
}

Int32 UI::GTK::GTKCore::GetScrollBarSize()
{
	return 16;
}

void UI::GTK::GTKCore::ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	UI::GTK::GTKMessageDialog::ShowOK(message, title, ctrl);
}

Bool UI::GTK::GTKCore::ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return UI::GTK::GTKMessageDialog::ShowYesNo(message, title, ctrl);
}

NN<UI::GUIButton> UI::GTK::GTKCore::NewButton(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::GTK::GTKButton> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKButton(*this, parent, text));
	return ctrl;
}

NN<UI::GUICheckBox> UI::GTK::GTKCore::NewCheckBox(NN<GUIClientControl> parent, Text::CStringNN text, Bool checked)
{
	NN<UI::GTK::GTKCheckBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKCheckBox(*this, parent, text, checked));
	return ctrl;
}

NN<UI::GUICheckedListBox> UI::GTK::GTKCore::NewCheckedListBox(NN<GUIClientControl> parent)
{
	NN<UI::GTK::GTKCheckedListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKCheckedListBox(*this, parent));
	return ctrl;
}

NN<UI::GUIComboBox> UI::GTK::GTKCore::NewComboBox(NN<GUIClientControl> parent, Bool allowEdit)
{
	NN<UI::GTK::GTKComboBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKComboBox(*this, parent, allowEdit));
	return ctrl;
}

NN<UI::GUIDateTimePicker> UI::GTK::GTKCore::NewDateTimePicker(NN<GUIClientControl> parent, Bool calendarSelect)
{
	NN<UI::GTK::GTKDateTimePicker> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKDateTimePicker(*this, parent));
	return ctrl;
}

NN<UI::GUIGroupBox> UI::GTK::GTKCore::NewGroupBox(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::GTK::GTKGroupBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKGroupBox(*this, parent, text));
	return ctrl;
}

NN<UI::GUIHScrollBar> UI::GTK::GTKCore::NewHScrollBar(NN<GUIClientControl> parent, Double width)
{
	NN<UI::GTK::GTKHScrollBar> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKHScrollBar(*this, parent, width));
	return ctrl;
}

NN<UI::GUILabel> UI::GTK::GTKCore::NewLabel(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::GTK::GTKLabel> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKLabel(*this, parent, text));
	return ctrl;
}

NN<UI::GUIListBox> UI::GTK::GTKCore::NewListBox(NN<GUIClientControl> parent, Bool multiSelect)
{
	NN<UI::GTK::GTKListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKListBox(*this, parent, multiSelect));
	return ctrl;
}

NN<UI::GUIPictureBox> UI::GTK::GTKCore::NewPictureBox(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize)
{
	NN<UI::GTK::GTKPictureBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKPictureBox(*this, parent, eng, hasBorder, allowResize));
	return ctrl;
}

NN<UI::GUIPictureBoxSimple> UI::GTK::GTKCore::NewPictureBoxSimple(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder)
{
	NN<UI::GTK::GTKPictureBoxSimple> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKPictureBoxSimple(*this, parent, eng, hasBorder));
	return ctrl;
}

NN<UI::GUIProgressBar> UI::GTK::GTKCore::NewProgressBar(NN<GUIClientControl> parent, UInt64 totalCnt)
{
	NN<UI::GTK::GTKProgressBar> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKProgressBar(*this, parent, totalCnt));
	return ctrl;
}

NN<UI::GUIRadioButton> UI::GTK::GTKCore::NewRadioButton(NN<GUIClientControl> parent, Text::CStringNN initText, Bool selected)
{
	NN<UI::GTK::GTKRadioButton> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKRadioButton(*this, parent, initText, selected));
	return ctrl;
}

NN<UI::GUIRealtimeLineChart> UI::GTK::GTKCore::NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS, Optional<Media::ColorSess> colorSess)
{
	NN<UI::GTK::GTKRealtimeLineChart> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKRealtimeLineChart(*this, parent, eng, lineCnt, sampleCnt, updateIntervalMS, colorSess));
	return ctrl;
}

NN<UI::GUITabControl> UI::GTK::GTKCore::NewTabControl(NN<GUIClientControl> parent)
{
	NN<UI::GTK::GTKTabControl> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKTabControl(*this, parent));
	return ctrl;
}

NN<UI::GUITextBox> UI::GTK::GTKCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText)
{
	NN<UI::GTK::GTKTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKTextBox(*this, parent, initText, false));
	return ctrl;
}

NN<UI::GUITextBox> UI::GTK::GTKCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine)
{
	NN<UI::GTK::GTKTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKTextBox(*this, parent, initText, multiLine));
	return ctrl;
}

NN<UI::GUITrackBar> UI::GTK::GTKCore::NewTrackBar(NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal)
{
	NN<UI::GTK::GTKTrackBar> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKTrackBar(*this, parent, minVal, maxVal, currVal));
	return ctrl;
}

NN<UI::GUIHSplitter> UI::GTK::GTKCore::NewHSplitter(NN<GUIClientControl> parent, Int32 width, Bool isRight)
{
	NN<UI::GTK::GTKHSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKHSplitter(*this, parent, width, isRight));
	return ctrl;
}

NN<UI::GUIVSplitter> UI::GTK::GTKCore::NewVSplitter(NN<GUIClientControl> parent, Int32 height, Bool isBottom)
{
	NN<UI::GTK::GTKVSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKVSplitter(*this, parent, height, isBottom));
	return ctrl;
}

NN<UI::GUIFileDialog> UI::GTK::GTKCore::NewFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave)
{
	NN<UI::GTK::GTKFileDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKFileDialog(compName, appName, dialogName, isSave));
	return ctrl;
}

NN<UI::GUIFolderDialog> UI::GTK::GTKCore::NewFolderDialog()
{
	NN<UI::GTK::GTKFolderDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKFolderDialog());
	return ctrl;
}

NN<UI::GUIFontDialog> UI::GTK::GTKCore::NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::GTK::GTKFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIFontDialog> UI::GTK::GTKCore::NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::GTK::GTKFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::GTK::GTKCore::NewPanelBase(NN<UI::GUIPanel> master, Optional<ControlHandle> parentHWnd)
{
	NN<UI::GTK::GTKPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKPanelBase(master, *this, parentHWnd));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::GTK::GTKCore::NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent)
{
	NN<UI::GTK::GTKPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::GTK::GTKPanelBase(master, *this, parent));
	return ctrl;
}
