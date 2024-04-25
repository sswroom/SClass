#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
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

UI::Win::WinCore::WinCore(InstanceHandle *hInst)
{
	this->hInst = hInst;
	this->focusWnd = 0;
	this->focusHAcc = 0;
	this->noDispOff = false;
	frmCnt = 0;
	this->hasCommCtrl = false;
	this->monMgr = 0;
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
		else if (this->focusHAcc == 0 || TranslateAccelerator((HWND)this->focusWnd, (HACCEL)this->focusHAcc, &msg) == 0)
		{
			if (!IsDialogMessage((HWND)this->focusWnd, &msg))
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
		if (this->focusHAcc == 0 || TranslateAccelerator((HWND)this->focusWnd, (HACCEL)this->focusHAcc, &msg) == 0)
		{
			if (!IsDialogMessage((HWND)this->focusWnd, &msg))
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

typedef HRESULT (WINAPI *GetDpiForMonitorFunc)(HMONITOR hmonitor, OSInt dpiType, UINT *dpiX, UINT *dpiY);

Double UI::Win::WinCore::GetMagnifyRatio(MonitorHandle *hMonitor)
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
			if (GetDpiForMonitorF((HMONITOR)hMonitor, 0, &xdpi, &ydpi) == 0)
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

void UI::Win::WinCore::SetFocusWnd(void *hWnd, void *hAcc)
{
	this->focusWnd = hWnd;
	this->focusHAcc = hAcc;
}

UI::InstanceHandle *UI::Win::WinCore::GetHInst()
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

Math::Size2D<UOSInt> UI::Win::WinCore::GetDesktopSize()
{
	return Math::Size2D<UOSInt>((UOSInt)(OSInt)GetSystemMetrics(SM_CXFULLSCREEN),
		(UOSInt)(OSInt)GetSystemMetrics(SM_CYFULLSCREEN));
}

Math::Coord2D<OSInt> UI::Win::WinCore::GetCursorPos()
{
	POINT pt;
	pt.x = 0;
	pt.y = 0;
	::GetCursorPos(&pt);
	return Math::Coord2D<OSInt>(pt.x, pt.y);
}

typedef BOOL (WINAPI* SETAUTOROTATION)(BOOL bEnable);

void UI::Win::WinCore::SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot)
{
	IO::Library lib((const UTF8Char*)"user32.dll");
	SETAUTOROTATION SetAutoRotation = (SETAUTOROTATION)lib.GetFuncNum(2507);
	if(SetAutoRotation != NULL)
	{
		SetAutoRotation(FALSE);
	}
}

void UI::Win::WinCore::GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi)
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

void UI::Win::WinCore::SetMonitorMgr(Media::MonitorMgr *monMgr)
{
	this->monMgr = monMgr;
}

Media::MonitorMgr *UI::Win::WinCore::GetMonitorMgr()
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
	NEW_CLASSNN(ctrl, UI::Win::WinHScrollBar(*this, parent, width));
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

NN<UI::GUIRealtimeLineChart> UI::Win::WinCore::NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS)
{
	NN<UI::Win::WinRealtimeLineChart> ctrl;
	NEW_CLASSNN(ctrl, UI::Win::WinRealtimeLineChart(*this, parent, eng, lineCnt, sampleCnt, updateIntervalMS));
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

NN<UI::GUITrackBar> UI::Win::WinCore::NewTrackBar(NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal)
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

NN<UI::GUIFileDialog> UI::Win::WinCore::NewFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave)
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

NN<UI::GUIPanelBase> UI::Win::WinCore::NewPanelBase(NN<UI::GUIPanel> master, ControlHandle *parentHWnd)
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

OSInt UI::Win::WinCore::MSGetWindowObj(ControlHandle *hWnd, OSInt index)
{
#ifdef _WIN32_WCE
	return (OSInt)GetWindowLong((HWND)hWnd, (int)index);
#else
	return (OSInt)GetWindowLongPtr((HWND)hWnd, (int)index);
#endif
}

OSInt UI::Win::WinCore::MSSetWindowObj(ControlHandle *hWnd, OSInt index, OSInt value)
{
#ifdef _WIN32_WCE
	return (OSInt)SetWindowLong((HWND)hWnd, (int)index, value);
#elif _OSINT_SIZE == 64
	return (OSInt)SetWindowLongPtr((HWND)hWnd, (int)index, value);
#else
	return (OSInt)SetWindowLongPtr((HWND)hWnd, (int)index, (LONG)value);
#endif
}

OSInt UI::Win::WinCore::MSSetClassObj(ControlHandle *hWnd, OSInt index, OSInt value)
{
#ifdef _WIN32_WCE
	return (OSInt)SetClassLong((HWND)hWnd, (int)index, value);
#elif _OSINT_SIZE == 64
	return (OSInt)SetClassLongPtr((HWND)hWnd, (int)index, value);
#else
	return (OSInt)SetClassLongPtr((HWND)hWnd, (int)index, (LONG)value);
#endif
}
