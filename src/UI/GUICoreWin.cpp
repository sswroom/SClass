#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/Library.h"
#include "Media/GDIEngineC.h"
#include "UI/GUICoreWin.h"
#ifdef _WIN32_WCE
#include "Sync/Thread.h"
#endif
#include <windows.h>

/////////////////////////////// EnumDisplayMonitors
/////////////////////////////// GetMonitorInfoW

UI::GUICoreWin::GUICoreWin(void *hInst)
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

UI::GUICoreWin::~GUICoreWin()
{
}

void UI::GUICoreWin::Run()
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

void UI::GUICoreWin::ProcessMessages()
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

void UI::GUICoreWin::WaitForMessages()
{
#ifndef _WIN32_WCE
	WaitMessage();
#else
    MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) == 0)
	{
		Sync::Thread::Sleep(10);
	}
#endif
}

void UI::GUICoreWin::Exit()
{
	PostQuitMessage(0);
}

Media::DrawEngine *UI::GUICoreWin::CreateDrawEngine()
{
	Media::DrawEngine *deng;
	NEW_CLASS(deng, Media::GDIEngineC());
	return deng;
}

typedef HRESULT (WINAPI *GetDpiForMonitorFunc)(HMONITOR hmonitor, OSInt dpiType, UINT *dpiX, UINT *dpiY);

Double UI::GUICoreWin::GetMagnifyRatio(void *hMonitor)
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

void UI::GUICoreWin::UseDevice(Bool useSystem, Bool useDisplay)
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

void UI::GUICoreWin::SetFocusWnd(void *hWnd, void *hAcc)
{
	this->focusWnd = hWnd;
	this->focusHAcc = hAcc;
}

void *UI::GUICoreWin::GetHInst()
{
	return this->hInst;
}

void UI::GUICoreWin::SetNoDisplayOff(Bool noDispOff)
{
	this->noDispOff = noDispOff;
}

void UI::GUICoreWin::DisplayOff()
{
	if (!this->noDispOff)
	{
#if !defined(_WIN32_WCE)
		SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
#endif
	}
}

void UI::GUICoreWin::Suspend()
{
#if !defined(_WIN32_WCE)
	SetSystemPowerState(TRUE, FALSE);
#endif
}

void UI::GUICoreWin::GetDesktopSize(UOSInt *w, UOSInt *h)
{
	*w = (UOSInt)(OSInt)GetSystemMetrics(SM_CXFULLSCREEN);
	*h = (UOSInt)(OSInt)GetSystemMetrics(SM_CYFULLSCREEN);
}

void UI::GUICoreWin::GetCursorPos(OSInt *x, OSInt *y)
{
	POINT pt;
	pt.x = 0;
	pt.y = 0;
	::GetCursorPos(&pt);
	*x = pt.x;
	*y = pt.y;
}

typedef BOOL (WINAPI* SETAUTOROTATION)(BOOL bEnable);

void UI::GUICoreWin::SetDisplayRotate(void *hMonitor, DisplayRotation rot)
{
	IO::Library lib((const UTF8Char*)"user32.dll");
	SETAUTOROTATION SetAutoRotation = (SETAUTOROTATION)lib.GetFuncNum(2507);
	if(SetAutoRotation != NULL)
	{
		SetAutoRotation(FALSE);
	}
}

void UI::GUICoreWin::GetMonitorDPIs(void *hMonitor, Double *hdpi, Double *ddpi)
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

void UI::GUICoreWin::SetMonitorMgr(Media::MonitorMgr *monMgr)
{
	this->monMgr = monMgr;
}

OSInt UI::GUICoreWin::MSGetWindowObj(void *hWnd, OSInt index)
{
#ifdef _WIN32_WCE
	return (OSInt)GetWindowLong((HWND)hWnd, (int)index);
#else
	return (OSInt)GetWindowLongPtr((HWND)hWnd, (int)index);
#endif
}

OSInt UI::GUICoreWin::MSSetWindowObj(void *hWnd, OSInt index, OSInt value)
{
#ifdef _WIN32_WCE
	return (OSInt)SetWindowLong((HWND)hWnd, (int)index, value);
#elif _OSINT_SIZE == 64
	return (OSInt)SetWindowLongPtr((HWND)hWnd, (int)index, value);
#else
	return (OSInt)SetWindowLongPtr((HWND)hWnd, (int)index, (LONG)value);
#endif
}

OSInt UI::GUICoreWin::MSSetClassObj(void *hWnd, OSInt index, OSInt value)
{
#ifdef _WIN32_WCE
	return (OSInt)SetClassLong((HWND)hWnd, (int)index, value);
#elif _OSINT_SIZE == 64
	return (OSInt)SetClassLongPtr((HWND)hWnd, (int)index, value);
#else
	return (OSInt)SetClassLongPtr((HWND)hWnd, (int)index, (LONG)value);
#endif
}
