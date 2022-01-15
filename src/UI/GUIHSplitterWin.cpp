#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIHSplitter.h"
#include <windows.h>

#define CLASSNAME L"HSplitter"
Int32 UI::GUIHSplitter::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#if defined(_WIN32_WCE)
#define GetWindowLongPtr(a, b) GetWindowLongW(a, b)
#endif

OSInt __stdcall UI::GUIHSplitter::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIHSplitter *me = (UI::GUIHSplitter*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	POINT pt;
	HDC hdc;
	UOSInt i;
	UI::GUIControl *ctrl;

	if (me == 0)
	{
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	switch (msg)
	{
	case WM_PAINT:
		break;
	case WM_LBUTTONDOWN:
#ifdef _WIN32_WCE
		pt.x = (Int16)LOWORD(lParam);
		pt.y = (Int16)HIWORD(lParam);
#else
		GetCursorPos(&pt);
#endif
		me->dragMode = true;
		me->dragX = pt.x;
		me->dragY = pt.y;
		me->lastX = pt.x;
		me->lastY = pt.y;
		me->CalDragRange();
		me->SetCapture();
		
		hdc = GetDC((HWND)me->parent->GetHandle());
		me->DrawXorBar(hdc, pt.x, pt.y);
		ReleaseDC((HWND)me->parent->GetHandle(), hdc);

		return 0;
	case WM_LBUTTONUP:
		if (me->dragMode)
		{
			OSInt drawX;
			OSInt x;
			OSInt y;
			UOSInt w;
			UOSInt h;
			Bool foundThis;
			UI::GUIControl::DockType dockType;

			me->dragMode = false;
			me->ReleaseCapture();

			hdc = GetDC((HWND)me->parent->GetHandle());
			me->DrawXorBar(hdc, me->lastX, me->lastY);
			ReleaseDC((HWND)me->parent->GetHandle(), hdc);

			me->GetPositionP(&x, 0);
			drawX = x + me->lastX - me->dragX;
			if (drawX < me->dragMin)
			{
				drawX = me->dragMin;
			}
			else if (drawX > me->dragMax)
			{
				drawX = me->dragMax;
			}
			foundThis = false;
			i = me->parent->GetChildCount();
			while (i-- > 0)
			{
				ctrl = me->parent->GetChild(i);
				if (ctrl == me)
				{
					foundThis = true;
				}
				else if (foundThis)
				{
					dockType = ctrl->GetDockType();
					if (dockType == UI::GUIControl::DOCK_RIGHT && me->isRight)
					{
						ctrl->GetPositionP(&x, &y);
						ctrl->GetSizeP(&w, &h);
						ctrl->SetAreaP(drawX, y, x + (OSInt)w, y + (OSInt)h, false);
						me->parent->UpdateChildrenSize(true);
						break;
					}
					else if (dockType == UI::GUIControl::DOCK_LEFT && !me->isRight)
					{
						ctrl->GetPositionP(&x, &y);
						ctrl->GetSizeP(&w, &h);
						ctrl->SetAreaP(x, y, drawX, y + (OSInt)h, false);
						me->parent->UpdateChildrenSize(true);
						break;
					}
				}
			}
		}
		return 0;
	case WM_MOUSEMOVE:
		if (me->dragMode)
		{
#ifdef _WIN32_WCE
			pt.x = (Int16)LOWORD(lParam);
			pt.y = (Int16)HIWORD(lParam);
#else
			GetCursorPos(&pt);
#endif

			hdc = GetDC((HWND)me->parent->GetHandle());

			me->DrawXorBar(hdc, me->lastX, me->lastY);
			me->DrawXorBar(hdc, pt.x, pt.y);

			ReleaseDC((HWND)me->parent->GetHandle(), hdc);
			me->lastX = pt.x;
			me->lastY = pt.y;
		}
		return 0;
	case WM_MOUSEWHEEL:
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUIHSplitter::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIHSplitter::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE)0, IDC_SIZEWE); 
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUIHSplitter::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::GUIHSplitter::DrawXorBar(void *hdc, Int32 x, Int32 y)
{
	static UInt16 _dotPatternBmp[8] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};

	HBITMAP hbm;
	HBRUSH hbr;
	HBRUSH hbrushOld;
	OSInt drawX;

	OSInt posX;
	OSInt posY;
	UOSInt w;
	UOSInt h;
	this->GetPositionP(&posX, &posY);
	this->GetSizeP(&w, &h);
	drawX = posX + x - this->dragX;
	if (drawX < dragMin)
	{
		drawX = dragMin;
	}
	else if (drawX > dragMax)
	{
		drawX = dragMax;
	}

	hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);
	hbr = CreatePatternBrush(hbm);

	SetBrushOrgEx((HDC)hdc, (int)drawX, (int)posY, 0);
	hbrushOld = (HBRUSH)SelectObject((HDC)hdc, hbr);

	PatBlt((HDC)hdc, (int)drawX, (int)posY, (int)w, (int)h, PATINVERT);
	
	SelectObject((HDC)hdc, hbrushOld);
	DeleteObject(hbr);
	DeleteObject(hbm);
}

void UI::GUIHSplitter::CalDragRange()
{
	UOSInt i;
	OSInt max;
	OSInt min;
	UOSInt tmp;
	Bool foundLeft = false;
	Bool foundRight = false;
	Bool foundThis = false;
	UI::GUIControl *ctrl;
	UI::GUIControl::DockType dockType;
	min = 0;
	Double maxD;
	this->parent->GetClientSize(&maxD, 0);
	max = Double2Int32(maxD * this->hdpi / 96.0);

	i = this->parent->GetChildCount();
	if (this->isRight)
	{
		while (i-- > 0)
		{
			ctrl = this->parent->GetChild(i);
			if (ctrl == this)
			{
				foundThis = true;
			}
			else
			{
				dockType = ctrl->GetDockType();
				if (dockType == UI::GUIControl::DOCK_RIGHT)
				{
					if (foundThis && !foundRight)
					{
						foundRight = true;
						ctrl->GetPositionP(&max, 0);
						ctrl->GetSizeP(&tmp, 0);
						max += (OSInt)tmp;
					}
				}
				else if (dockType == UI::GUIControl::DOCK_LEFT)
				{
					if (!foundLeft)
					{
						foundLeft = true;
						ctrl->GetPositionP(&min, 0);
						ctrl->GetSizeP(&tmp, 0);
						min += (OSInt)tmp;
					}
				}
			}
		}
	}
	else
	{
		while (i-- > 0)
		{
			ctrl = this->parent->GetChild(i);
			if (ctrl == this)
			{
				foundThis = true;
			}
			else
			{
				dockType = ctrl->GetDockType();
				if (dockType == UI::GUIControl::DOCK_RIGHT)
				{
					if (!foundRight)
					{
						foundRight = true;
						ctrl->GetPositionP(&max, 0);
					}
				}
				else if (dockType == UI::GUIControl::DOCK_LEFT)
				{
					if (foundThis && !foundLeft)
					{
						foundLeft = true;
						ctrl->GetPositionP(&min, 0);
					}
				}
			}
		}
	}
	this->dragMax = max;
	this->dragMin = min;
}

UI::GUIHSplitter::GUIHSplitter(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 width, Bool isRight) : UI::GUIControl(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui)->GetHInst());
	}

	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)this->ui)->GetHInst(), parent, CLASSNAME, 0, style, 0, 0, 0, width, 10);
	this->SetDockType(isRight?DOCK_RIGHT:DOCK_LEFT);
}

UI::GUIHSplitter::~GUIHSplitter()
{
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
}

const UTF8Char *UI::GUIHSplitter::GetObjectClass()
{
	return (const UTF8Char*)"HSplitter";
}

OSInt UI::GUIHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
