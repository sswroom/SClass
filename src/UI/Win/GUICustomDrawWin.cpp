#include "Stdafx.h"
#if defined(WINVER)
#undef WINVER
#endif
#define WINVER 0x601

#if defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x601

#include "MyMemory.h"
#include "IO/Library.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/PieArea.h"
#include "Math/Geometry/Polyline.h"
#include "Media/GDIEngine.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICustomDraw.h"
#include "UI/Win/WinCore.h"
#include <windows.h>

#define CLASSNAME L"CustomDraw"
Int32 UI::GUICustomDraw::useCnt = 0;

typedef BOOL (WINAPI *GetGestureInfoFunc)(HGESTUREINFO hGestureInfo, PGESTUREINFO pGestureInfo);
typedef BOOL (WINAPI *CloseGestureInfoHandleFunc)(HGESTUREINFO hGestureInfo);


#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

IntOS __stdcall UI::GUICustomDraw::FormWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)(IntOS)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	if (me == 0)
	{
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	switch (msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint((HWND)me->hwnd.OrNull(), &ps);
			NN<Media::DrawImage> dimg;
			dimg = NN<Media::GDIEngine>::ConvertFrom(me->eng)->CreateImageScn(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, me->colorSess);
			dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			me->OnDraw(dimg);
			me->eng->DeleteImage(dimg);
			EndPaint((HWND)me->hwnd.OrNull(), &ps);
		}
		return 0;
	case WM_LBUTTONDOWN:
		me->OnMouseDown(Math::Coord2D<IntOS>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), UI::GUIControl::MBTN_LEFT);
		return 0;
	case WM_LBUTTONUP:
		me->OnMouseUp(Math::Coord2D<IntOS>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), UI::GUIControl::MBTN_LEFT);
		return 0;
	case WM_MOUSEMOVE:
		me->OnMouseMove(Math::Coord2D<IntOS>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)));
		return 0;
	case WM_MOUSEWHEEL:
		{
			RECT rcWnd;
			GetWindowRect((HWND)hWnd, &rcWnd);
			me->OnMouseWheel(Math::Coord2D<IntOS>((Int16)LOWORD(lParam) - rcWnd.left, (Int16)HIWORD(lParam) - rcWnd.top), (Int16)HIWORD(wParam));
		}
		return 0;
	case WM_GESTURE:
		{
			GetGestureInfoFunc GetGestureInfoF = (GetGestureInfoFunc)me->lib->GetFunc("GetGestureInfo");
			CloseGestureInfoHandleFunc CloseGestureInfoHandleF = (CloseGestureInfoHandleFunc)me->lib->GetFunc("CloseGestureInfoHandle");
			if (GetGestureInfoF && CloseGestureInfoHandleF)
			{
				Bool handled = false;
				GESTUREINFO gi;  
				MemClear(&gi, sizeof(GESTUREINFO));
				gi.cbSize = sizeof(GESTUREINFO);
				if (GetGestureInfoF((HGESTUREINFO)lParam, &gi))
				{
					if (gi.dwID == GID_ZOOM)
					{
						if (gi.dwFlags & GF_BEGIN)
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGestureBegin(Math::Coord2D<IntOS>(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top), gi.ullArguments);
						}
						else if (gi.dwFlags & GF_END)
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGestureEnd(Math::Coord2D<IntOS>(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top), gi.ullArguments);
						}
						else
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGestureStep(Math::Coord2D<IntOS>(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top), gi.ullArguments);
						}
					}
				}
				CloseGestureInfoHandleF((HGESTUREINFO)lParam);
				if (handled)
				{
					return 0;
				}
				else
				{
					return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
				}
			}
		}
		break;
	case WM_SETFOCUS:
		me->focusing = true;
		break;
	case WM_KILLFOCUS:
		me->focusing = false;
		break;
	case WM_TIMER:
		if (me->focusing && me->joystickId != 0)
		{
			MMRESULT ret;
			JOYINFOEX info;
			info.dwSize = sizeof(JOYINFOEX);
			info.dwFlags = JOY_RETURNALL;
			ret = joyGetPosEx((UINT)(me->joystickId - 1), &info);
			if (ret == JOYERR_NOERROR)
			{
				IntOS i = 1;
				UInt32 mask = 1;
				while (i <= 32)
				{
					if ((me->jsLastButtons & mask) != (info.dwButtons & mask))
					{
						if (info.dwButtons & mask)
						{
							me->OnJSButtonDown(i);
						}
						else
						{
							me->OnJSButtonUp(i);
						}
					}
					mask = mask << 1;
					i++;
				}
				me->OnJSAxis((Int32)(info.dwXpos - 0x8000), (Int32)(info.dwYpos - 0x8000), (Int32)(info.dwZpos - 0x8000), (Int32)(info.dwRpos - 0x8000));
				me->jsLastButtons = info.dwButtons;
			}
		}
		me->OnTimerTick();
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_KEYDOWN:
		if (me->OnKeyDown(me->OSKey2GUIKey((UInt32)wParam)) == UI::EventState::StopEvent)
		{
			return 0;
		}
		break;
	case WM_SIZE:
		if (!me->selfResize)
		{
			me->OnSizeChanged(false);
		}
		return 0;//DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUICustomDraw::Init(Optional<InstanceHandle> hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUICustomDraw::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst.OrNull();
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUICustomDraw::Deinit(Optional<InstanceHandle> hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst.OrNull());
}

void UI::GUICustomDraw::InitJS()
{
	UInt32 nPad = joyGetNumDevs();
	UInt32 i;
	MMRESULT ret;
	if (nPad > 0)
	{
		JOYINFOEX info;
		i = 0;
		while (i < nPad)
		{
			info.dwSize = sizeof(JOYINFOEX);
			info.dwFlags = JOY_RETURNALL;

			ret = joyGetPosEx(i, &info);
			if (ret == JOYERR_NOERROR)
			{
				this->joystickId = i + 1;
				break;
			}
			i++;
		}
	}
}

UI::GUICustomDraw::GUICustomDraw(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Optional<Media::ColorSess> colorSess) : UI::GUIControl(ui, parent)
{
	this->eng = eng;
	this->colorSess = colorSess;
	NEW_CLASSNN(this->lib, IO::Library((const UTF8Char*)"User32.dll"));
	this->focusing = false;

	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"MapControl", style, 0, 0, 0, 640, 480);
	this->InitJS();
	SetTimer((HWND)this->hwnd.OrNull(), 1000, 18, 0);
}

UI::GUICustomDraw::~GUICustomDraw()
{
	this->lib.Delete();
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}

Text::CStringNN UI::GUICustomDraw::GetObjectClass() const
{
	return CSTR("CustomDraw");
}

IntOS UI::GUICustomDraw::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

UI::EventState UI::GUICustomDraw::OnMouseDown(Math::Coord2D<IntOS> scnPos, MouseButton btn)
{
	return UI::EventState::ContinueEvent;
}

UI::EventState UI::GUICustomDraw::OnMouseUp(Math::Coord2D<IntOS> scnPos, MouseButton btn)
{
	return UI::EventState::ContinueEvent;
}

void UI::GUICustomDraw::OnMouseMove(Math::Coord2D<IntOS> scnPos)
{
}

UI::EventState UI::GUICustomDraw::OnMouseWheel(Math::Coord2D<IntOS> scnPos, Int32 delta)
{
	return UI::EventState::ContinueEvent;
}

void UI::GUICustomDraw::OnGestureBegin(Math::Coord2D<IntOS> scnPos, UInt64 dist)
{
}

void UI::GUICustomDraw::OnGestureStep(Math::Coord2D<IntOS> scnPos, UInt64 dist)
{
}

void UI::GUICustomDraw::OnGestureEnd(Math::Coord2D<IntOS> scnPos, UInt64 dist)
{
}

void UI::GUICustomDraw::OnJSButtonDown(IntOS buttonId)
{
}

void UI::GUICustomDraw::OnJSButtonUp(IntOS buttonId)
{
}

void UI::GUICustomDraw::OnJSAxis(IntOS axis1, IntOS axis2, IntOS axis3, IntOS axis4)
{
}

UI::EventState UI::GUICustomDraw::OnKeyDown(UI::GUIControl::GUIKey key)
{
	return UI::EventState::ContinueEvent;
}

void UI::GUICustomDraw::OnTimerTick()
{
}

void UI::GUICustomDraw::OnDraw(NN<Media::DrawImage> img)
{
}
