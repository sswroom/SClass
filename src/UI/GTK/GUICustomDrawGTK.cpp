#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Math/Math.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/PieArea.h"
#include "Math/Geometry/Polyline.h"
#include "Media/GTKDrawEngine.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICustomDraw.h"
#include <gtk/gtk.h>
#include <stdio.h>

Int32 UI::GUICustomDraw::useCnt = 0;
struct UI::GUICustomDraw::ClassData
{
	guint timerId;
};

Int32 GUICustomDraw_OnTick(void *userObj)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)userObj;
	me->OnTimerTick();
	return 1;
}

gboolean GUICustomDraw_OnDraw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)data;
	NN<Media::DrawImage> dimg = ((Media::GTKDrawEngine*)me->eng.Ptr())->CreateImageScn(cr, Math::Coord2D<OSInt>(0, 0), Math::Coord2D<OSInt>(gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget)), me->colorSess);
	dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
	dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
	me->OnDraw(dimg);
	me->eng->DeleteImage(dimg);
	return true;
}

gboolean GUICustomDraw_OnMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_PRESS)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		me->OnMouseDown(Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)), btn);
	}
	return false;
}

gboolean GUICustomDraw_OnMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_RELEASE)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		me->OnMouseUp(Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)), btn);
	}
	return false;
}

gboolean GUICustomDraw_OnMouseMove(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->OnMouseMove(Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)));
	return false;
}

gboolean GUICustomDraw_OnMouseWheel(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)data;
	GdkEventScroll *evt = (GdkEventScroll*)event;
	return me->OnMouseWheel(Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)), Double2Int32(-evt->delta_y));
}

gboolean GUICustomDraw_OnKeyDown(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)user_data;
	GdkEventKey *evt = (GdkEventKey*)event;
	if (me->OnKeyDown(me->OSKey2GUIKey(evt->keyval)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
/*OSInt __stdcall UI::GUICustomDraw::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUICustomDraw *me = (UI::GUICustomDraw*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	if (me == 0)
	{
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	switch (msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint((HWND)me->hwnd, &ps);
			Media::DrawImage *dimg = ((Media::GDIEngine*)me->eng)->CreateImageScn(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
			dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			me->OnDraw(dimg);
			me->eng->DeleteImage(dimg);
			EndPaint((HWND)me->hwnd, &ps);
		}
		return 0;
	case WM_LBUTTONDOWN:
		me->OnMouseDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), UI::GUIControl::MBTN_LEFT);
		return 0;
	case WM_LBUTTONUP:
		me->OnMouseUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), UI::GUIControl::MBTN_LEFT);
		return 0;
	case WM_MOUSEMOVE:
		me->OnMouseMove((Int16)LOWORD(lParam), (Int16)HIWORD(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		{
			RECT rcWnd;
			GetWindowRect((HWND)hWnd, &rcWnd);
			me->OnMouseWheel((Int16)LOWORD(lParam) - rcWnd.left, (Int16)HIWORD(lParam) - rcWnd.top, (Int16)HIWORD(wParam));
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
							me->OnGestureBegin(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top, gi.ullArguments);
						}
						else if (gi.dwFlags & GF_END)
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGestureEnd(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top, gi.ullArguments);
						}
						else
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGestureStep(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top, gi.ullArguments);
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
			ret = joyGetPosEx(me->joystickId - 1, &info);
			if (ret == JOYERR_NOERROR)
			{
				OSInt i = 1;
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
	case WM_SIZE:
		if (!me->selfResize)
		{
			me->OnSizeChanged(false);
		}
		return 0;//DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUICustomDraw::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUICustomDraw::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUICustomDraw::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}*/

void UI::GUICustomDraw::InitJS()
{
/*	UInt32 nPad = joyGetNumDevs();
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
	}*/
}

UI::GUICustomDraw::GUICustomDraw(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Optional<Media::ColorSess> colorSess) : UI::GUIControl(ui, parent)
{
	this->eng = eng;
	this->colorSess = colorSess;
	NN<ClassData> data = MemAllocNN(ClassData);
	this->clsData = data;

	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "draw", G_CALLBACK(GUICustomDraw_OnDraw), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-press-event", G_CALLBACK(GUICustomDraw_OnMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-release-event", G_CALLBACK(GUICustomDraw_OnMouseUp), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "motion-notify-event", G_CALLBACK(GUICustomDraw_OnMouseMove), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "scroll-event", G_CALLBACK(GUICustomDraw_OnMouseWheel), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "key-press-event", G_CALLBACK(GUICustomDraw_OnKeyDown), this);
	gtk_widget_set_events((GtkWidget*)this->hwnd.OrNull(), GDK_ALL_EVENTS_MASK);
	gtk_widget_set_can_focus((GtkWidget*)this->hwnd.OrNull(), true);
	parent->AddChild(*this);
	this->Show();
	data->timerId = g_timeout_add(50, GUICustomDraw_OnTick, this);
}

UI::GUICustomDraw::~GUICustomDraw()
{
	NN<ClassData> data = this->clsData;
	g_source_remove(data->timerId);
	MemFreeNN(data);
}

Text::CStringNN UI::GUICustomDraw::GetObjectClass() const
{
	return CSTR("CustomDraw");
}

OSInt UI::GUICustomDraw::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::GUICustomDraw::OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	return false;
}

Bool UI::GUICustomDraw::OnMouseUp(Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	return false;
}

void UI::GUICustomDraw::OnMouseMove(Math::Coord2D<OSInt> scnPos)
{
}

Bool UI::GUICustomDraw::OnMouseWheel(Math::Coord2D<OSInt> scnPos, Int32 delta)
{
	return false;
}

void UI::GUICustomDraw::OnGestureBegin(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
}

void UI::GUICustomDraw::OnGestureStep(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
}

void UI::GUICustomDraw::OnGestureEnd(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
}

void UI::GUICustomDraw::OnJSButtonDown(OSInt buttonId)
{
}

void UI::GUICustomDraw::OnJSButtonUp(OSInt buttonId)
{
}

void UI::GUICustomDraw::OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4)
{
}

Bool UI::GUICustomDraw::OnKeyDown(UI::GUIControl::GUIKey key)
{
	return false;
}

void UI::GUICustomDraw::OnTimerTick()
{
}

void UI::GUICustomDraw::OnDraw(NN<Media::DrawImage> img)
{
}
