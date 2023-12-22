#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomOS.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinRealtimeLineChart.h"
#include <windows.h>

#define CLASSNAME L"RealtimeLineChart"
Int32 UI::Win::WinRealtimeLineChart::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::Win::WinRealtimeLineChart::RLCWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::Win::WinRealtimeLineChart *me = (UI::Win::WinRealtimeLineChart*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	switch (msg)
	{
	case WM_SIZE:
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_PAINT:
		{
			RECT rc;
			PAINTSTRUCT ps;
			GetClientRect((HWND)hWnd, &rc);
			BeginPaint((HWND)hWnd, &ps);
			NotNullPtr<Media::DrawImage> scn;
			if (scn.Set(NotNullPtr<Media::GDIEngine>::ConvertFrom(me->eng)->CreateImageScn(ps.hdc, rc.left, rc.top, rc.right, rc.bottom)))
			{
				me->OnPaint(scn);
				me->eng->DeleteImage(scn);
			}
			EndPaint((HWND)hWnd, &ps);
		}
		return 0;
	case WM_TIMER:
		if (me->valueChanged)
		{
			me->valueChanged = false;
			me->Redraw();
		}
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::Win::WinRealtimeLineChart::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::Win::WinRealtimeLineChart::RLCWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = 0; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::Win::WinRealtimeLineChart::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

UI::Win::WinRealtimeLineChart::WinRealtimeLineChart(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateInterval) : UI::GUIRealtimeLineChart(ui, parent, eng, lineCnt, sampleCnt)
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);

	SetTimer((HWND)this->hwnd, 1, updateInterval, 0);
}

UI::Win::WinRealtimeLineChart::~WinRealtimeLineChart()
{
	KillTimer((HWND)this->hwnd, 1);
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}
