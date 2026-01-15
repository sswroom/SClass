#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Media/GDIEngine.h"
#include "UI/Win/WinButton.h"
#include "UI/Win/WinCheckBox.h"
#include "UI/Win/WinCheckedListBox.h"
#include "UI/Win/WinComboBox.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinDateTimePicker.h"
#include "UI/Win/WinFileDialog.h"
#include "UI/Win/WinFolderDialog.h"
#include "UI/Win/WinFontDialog.h"
#include "UI/Win/WinGroupBox.h"
#include "UI/Win/WinHScrollBar.h"
#include "UI/Win/WinHSplitter.h"
#include "UI/Win/WinLabel.h"
#include "UI/Win/WinListBox.h"
#include "UI/Win/WinMessageDialog.h"
#include "UI/Win/WinPanelBase.h"
#include "UI/Win/WinPictureBox.h"
#include "UI/Win/WinPictureBoxSimple.h"
#include "UI/Win/WinProgressBar.h"
#include "UI/Win/WinRadioButton.h"
#include "UI/Win/WinRealtimeLineChart.h"
#include "UI/Win/WinTabControl.h"
#include "UI/Win/WinTextBox.h"
#include "UI/Win/WinTrackBar.h"
#include "UI/Win/WinVSplitter.h"
#ifdef _WIN32_WCE
#include "Sync/ThreadUtil.h"
#endif
#include <windows.h>

/////////////////////////////// EnumDisplayMonitors
/////////////////////////////// GetMonitorInfoW

UI::Win::WinCore::WinCore(Optional<InstanceHandle> hInst)
{
	this->hInst = hInst;
	this->focusWnd = nullptr;
	this->focusHAcc = 0;
	this->noDispOff = false;
	frmCnt = 0;
	this->hasCommCtrl = false;
	this->monMgr = nullptr;
	// InitCommonControls();
}

UI::Win::WinCore::~WinCore()
{
}

void UI::Win::WinCore::Run()
{
    MSG msg;
    BOOL bRet; 

    while((bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
			break;
        }
		else if (this->focusHAcc == 0 || TranslateAccelerator((HWND)this->focusWnd.OrNull(), (HACCEL)this->focusHAcc, &msg) == 0)
		{
			if (!IsDialogMessage((HWND)this->focusWnd.OrNull(), &msg))
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
    } 
}

void UI::Win::WinCore::ProcessMessages()
{
    MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (this->focusHAcc == 0 || TranslateAccelerator((HWND)this->focusWnd.OrNull(), (HACCEL)this->focusHAcc, &msg) == 0)
		{
			if (!IsDialogMessage((HWND)this->focusWnd.OrNull(), &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
    }
}

void UI::Win::WinCore::WaitForMessages()
{
#ifndef _WIN32_WCE
	WaitMessage();
#else
    MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) == 0)
	{
		Sync::SimpleThread::Sleep(10);
	}
#endif
}

void UI::Win::WinCore::Exit()
{
	PostQuitMessage(0);
}

NN<Media::DrawEngine> UI::Win::WinCore::CreateDrawEngine()
{
	NN<Media::DrawEngine> deng;
	NEW_CLASSNN(deng, Media::GDIEngine());
	return deng;
}

typedef HRESULT (WINAPI *GetDpiForMonitorFunc)(HMONITOR hmonitor, IntOS dpiType, UINT *dpiX, UINT *dpiY);

Double UI::Win::WinCore::GetMagnifyRatio(Optional<MonitorHandle> hMonitor)
{
	Double osdpi = 0;
	IO::Library lib((const UTF8Char*)"Shcore.dll");
	if (!lib.IsError())
	{
		GetDpiForMonitorFunc GetDpiForMonitorF = (GetDpiForMonitorFunc)lib.GetFunc("GetDpiForMonitor");
		if (GetDpiForMonitorF != 0)
		{
			UINT xdpi;
			UINT ydpi;
			if (GetDpiForMonitorF((HMONITOR)hMonitor.OrNull(), 0, &xdpi, &ydpi) == 0)
			{
				osdpi = xdpi;
			}
		}
	}
	if (osdpi == 0)
	{
		HDC screen = GetDC(0);
		osdpi = GetDeviceCaps(screen,LOGPIXELSX);
		ReleaseDC(0, screen);
	}

	if (osdpi == 0)
		return 1.0;
	else
		return osdpi / 96.0;
}

void UI::Win::WinCore::UseDevice(Bool useSystem, Bool useDisplay)
{
#if !defined(_WIN32_WCE)
	if (useSystem)
	{
		SetThreadExecutionState(ES_SYSTEM_REQUIRED);
	}
	if (useDisplay)
	{
		SetThreadExecutionState(ES_DISPLAY_REQUIRED);
	}
	else if (useSystem && this->noDispOff)
	{
		SetThreadExecutionState(ES_DISPLAY_REQUIRED);
	}
#endif
}

void UI::Win::WinCore::SetFocusWnd(Optional<ControlHandle> hWnd, void *hAcc)
{
	this->focusWnd = hWnd;
	this->focusHAcc = hAcc;
}

Optional<UI::InstanceHandle> UI::Win::WinCore::GetHInst()
{
	return this->hInst;
}

void UI::Win::WinCore::SetNoDisplayOff(Bool noDispOff)
{
	this->noDispOff = noDispOff;
}

void UI::Win::WinCore::DisplayOff()
{
	if (!this->noDispOff)
	{
#if !defined(_WIN32_WCE)
		SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
#endif
	}
}

void UI::Win::WinCore::Suspend()
{
#if !defined(_WIN32_WCE)
	SetSystemPowerState(TRUE, FALSE);
#endif
}

Math::Size2D<UIntOS> UI::Win::WinCore::GetDesktopSize()
{
	return Math::Size2D<UIntOS>((UIntOS)(IntOS)GetSystemMetrics(SM_CXFULLSCREEN),
		(UIntOS)(IntOS)GetSystemMetrics(SM_CYFULLSCREEN));
}

Math::Coord2D<IntOS> UI::Win::WinCore::GetCursorPos()
{
	POINT pt;
	pt.x = 0;
	pt.y = 0;
	::GetCursorPos(&pt);
	return Math::Coord2D<IntOS>(pt.x, pt.y);
}

typedef BOOL (WINAPI* SETAUTOROTATION)(BOOL bEnable);

void UI::Win::WinCore::SetDisplayRotate(Optional<MonitorHandle> hMonitor, DisplayRotation rot)
{
	IO::Library lib((const UTF8Char*)"user32.dll");
	SETAUTOROTATION SetAutoRotation = (SETAUTOROTATION)lib.GetFuncNum(2507);
	if(SetAutoRotation != NULL)
	{
		SetAutoRotation(FALSE);
	}
}

void UI::Win::WinCore::GetMonitorDPIs(Optional<MonitorHandle> hMonitor, OutParam<Double> hdpi, OutParam<Double> ddpi)
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

void UI::Win::WinCore::SetMonitorMgr(Optional<Media::MonitorMgr> monMgr)
{
	this->monMgr = monMgr;
}

Optional<Media::MonitorMgr> UI::Win::WinCore::GetMonitorMgr()
{
	return this->monMgr;
}

Int32 UI::Win::WinCore::GetScrollBarSize()
{
	return GetSystemMetrics(SM_CYHSCROLL);
}

void UI::Win::WinCore::ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	UI::Win::WinMessageDialog::ShowOK(message, title, ctrl);
}

Bool UI::Win::WinCore::ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return UI::Win::WinMessageDialog::ShowYesNo(message, title, ctrl);
}

NN<UI::GUIButton> UI::Win::WinCore::NewButton(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::Win::WinButton> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinButton(*this, parent, text));
	return ctrl;
}

NN<UI::GUICheckBox> UI::Win::WinCore::NewCheckBox(NN<GUIClientControl> parent, Text::CStringNN text, Bool checked)
{
	NN<UI::Win::WinCheckBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinCheckBox(*this, parent, text, checked));
	return ctrl;
}

NN<UI::GUICheckedListBox> UI::Win::WinCore::NewCheckedListBox(NN<GUIClientControl> parent)
{
	NN<UI::Win::WinCheckedListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinCheckedListBox(*this, parent));
	return ctrl;
}

NN<UI::GUIComboBox> UI::Win::WinCore::NewComboBox(NN<GUIClientControl> parent, Bool allowEdit)
{
	NN<UI::Win::WinComboBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinComboBox(*this, parent, allowEdit));
	return ctrl;
}

NN<UI::GUIDateTimePicker> UI::Win::WinCore::NewDateTimePicker(NN<GUIClientControl> parent, Bool calendarSelect)
{
	NN<UI::Win::WinDateTimePicker> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinDateTimePicker(*this, parent, calendarSelect));
	return ctrl;
}

NN<UI::GUIGroupBox> UI::Win::WinCore::NewGroupBox(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::Win::WinGroupBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinGroupBox(*this, parent, text));
	return ctrl;
}

NN<UI::GUIHScrollBar> UI::Win::WinCore::NewHScrollBar(NN<GUIClientControl> parent, Double width)
{
	NN<UI::Win::WinHScrollBar> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinHScrollBar(*this, parent, Double2Int32(width)));
	return ctrl;
}

NN<UI::GUILabel> UI::Win::WinCore::NewLabel(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::Win::WinLabel> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinLabel(*this, parent, text));
	return ctrl;
}

NN<UI::GUIListBox> UI::Win::WinCore::NewListBox(NN<GUIClientControl> parent, Bool multiSelect)
{
	NN<UI::Win::WinListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinListBox(*this, parent, multiSelect));
	return ctrl;
}

NN<UI::GUIPictureBox> UI::Win::WinCore::NewPictureBox(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize)
{
	NN<UI::Win::WinPictureBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinPictureBox(*this, parent, eng, hasBorder, allowResize));
	return ctrl;
}

NN<UI::GUIPictureBoxSimple> UI::Win::WinCore::NewPictureBoxSimple(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder)
{
	NN<UI::Win::WinPictureBoxSimple> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinPictureBoxSimple(*this, parent, eng, hasBorder));
	return ctrl;
}

NN<UI::GUIProgressBar> UI::Win::WinCore::NewProgressBar(NN<GUIClientControl> parent, UInt64 totalCnt)
{
	NN<UI::Win::WinProgressBar> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinProgressBar(*this, parent, totalCnt));
	return ctrl;
}

NN<UI::GUIRadioButton> UI::Win::WinCore::NewRadioButton(NN<GUIClientControl> parent, Text::CStringNN initText, Bool selected)
{
	NN<UI::Win::WinRadioButton> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinRadioButton(*this, parent, initText, selected));
	return ctrl;
}

NN<UI::GUIRealtimeLineChart> UI::Win::WinCore::NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UIntOS lineCnt, UIntOS sampleCnt, UInt32 updateIntervalMS, Optional<Media::ColorSess> colorSess)
{
	NN<UI::Win::WinRealtimeLineChart> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinRealtimeLineChart(*this, parent, eng, colorSess, lineCnt, sampleCnt, updateIntervalMS));
	return ctrl;
}

NN<UI::GUITabControl> UI::Win::WinCore::NewTabControl(NN<GUIClientControl> parent)
{
	NN<UI::Win::WinTabControl> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinTabControl(*this, parent));
	return ctrl;
}

NN<UI::GUITextBox> UI::Win::WinCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText)
{
	NN<UI::Win::WinTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinTextBox(*this, parent, initText, false));
	return ctrl;
}

NN<UI::GUITextBox> UI::Win::WinCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine)
{
	NN<UI::Win::WinTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinTextBox(*this, parent, initText, multiLine));
	return ctrl;
}

NN<UI::GUITrackBar> UI::Win::WinCore::NewTrackBar(NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal)
{
	NN<UI::Win::WinTrackBar> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinTrackBar(*this, parent, minVal, maxVal, currVal));
	return ctrl;
}

NN<UI::GUIHSplitter> UI::Win::WinCore::NewHSplitter(NN<GUIClientControl> parent, Int32 width, Bool isRight)
{
	NN<UI::Win::WinHSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinHSplitter(*this, parent, width, isRight));
	return ctrl;
}

NN<UI::GUIVSplitter> UI::Win::WinCore::NewVSplitter(NN<GUIClientControl> parent, Int32 height, Bool isBottom)
{
	NN<UI::Win::WinVSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinVSplitter(*this, parent, height, isBottom));
	return ctrl;
}

NN<UI::GUIFileDialog> UI::Win::WinCore::NewFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave)
{
	NN<UI::Win::WinFileDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinFileDialog(compName, appName, dialogName, isSave));
	return ctrl;
}

NN<UI::GUIFolderDialog> UI::Win::WinCore::NewFolderDialog()
{
	NN<UI::Win::WinFolderDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinFolderDialog());
	return ctrl;
}

NN<UI::GUIFontDialog> UI::Win::WinCore::NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::Win::WinFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIFontDialog> UI::Win::WinCore::NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::Win::WinFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::Win::WinCore::NewPanelBase(NN<UI::GUIPanel> master, Optional<ControlHandle> parentHWnd)
{
	NN<UI::Win::WinPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinPanelBase(master, *this, parentHWnd));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::Win::WinCore::NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent)
{
	NN<UI::Win::WinPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinPanelBase(master, *this, parent));
	return ctrl;
}

Bool UI::Win::WinCore::IsForwarded()
{
	return false;
}

IntOS UI::Win::WinCore::MSGetWindowObj(Optional<ControlHandle> hWnd, IntOS index)
{
#ifdef _WIN32_WCE
	return (IntOS)GetWindowLong((HWND)hWnd.OrNull(), (int)index);
#else
	return (IntOS)GetWindowLongPtr((HWND)hWnd.OrNull(), (int)index);
#endif
}

IntOS UI::Win::WinCore::MSSetWindowObj(Optional<ControlHandle> hWnd, IntOS index, IntOS value)
{
#ifdef _WIN32_WCE
	return (IntOS)SetWindowLong((HWND)hWnd.OrNull(), (int)index, value);
#elif _OSINT_SIZE == 64
	return (IntOS)SetWindowLongPtr((HWND)hWnd.OrNull(), (int)index, value);
#else
	return (IntOS)SetWindowLongPtr((HWND)hWnd.OrNull(), (int)index, (LONG)value);
#endif
}

IntOS UI::Win::WinCore::MSSetClassObj(Optional<ControlHandle> hWnd, IntOS index, IntOS value)
{
#ifdef _WIN32_WCE
	return (IntOS)SetClassLong((HWND)hWnd.OrNull(), (int)index, value);
#elif _OSINT_SIZE == 64
	return (IntOS)SetClassLongPtr((HWND)hWnd.OrNull(), (int)index, value);
#else
	return (IntOS)SetClassLongPtr((HWND)hWnd.OrNull(), (int)index, (LONG)value);
#endif
}
