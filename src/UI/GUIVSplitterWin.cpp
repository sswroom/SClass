#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIVSplitter.h"
#include <windows.h>

#define CLASSNAME L"VSplitter"
Int32 UI::GUIVSplitter::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#if defined(_WIN32_WCE)
#define GetWindowLongPtr(a, b) GetWindowLongW(a, b)
#endif

OSInt __stdcall UI::GUIVSplitter::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIVSplitter *me = (UI::GUIVSplitter*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	POINT pt;
	HDC hdc;
	OSInt i;
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
		GetCursorPos(&pt);
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
			OSInt drawY;
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

			me->GetPositionP(0, &y);
			drawY = y + me->lastY - me->dragY;
			if (drawY < me->dragMin)
			{
				drawY = me->dragMin;
			}
			else if (drawY > me->dragMax)
			{
				drawY = me->dragMax;
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
					if (dockType == UI::GUIControl::DOCK_BOTTOM && me->isBottom)
					{
						ctrl->GetPositionP(&x, &y);
						ctrl->GetSizeP(&w, &h);
						ctrl->SetAreaP(x, drawY, x + w, y + h, false);
						me->parent->UpdateChildrenSize(true);
						break;
					}
					else if (dockType == UI::GUIControl::DOCK_TOP && !me->isBottom)
					{
						ctrl->GetPositionP(&x, &y);
						ctrl->GetSizeP(&w, &h);
						ctrl->SetAreaP(x, y, x + w, drawY, false);
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
			GetCursorPos(&pt);

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

void UI::GUIVSplitter::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIVSplitter::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE)0, IDC_SIZENS); 
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUIVSplitter::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::GUIVSplitter::DrawXorBar(void *hdc, Int32 x, Int32 y)
{
	static UInt16 _dotPatternBmp[8] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};

	HBITMAP hbm;
	HBRUSH hbr;
	HBRUSH hbrushOld;
	OSInt drawY;

	OSInt cliOfstX = 0;
	OSInt cliOfstY = 0;
	Double lcliOfstX = 0;
	Double lcliOfstY = 0;
	OSInt posX;
	OSInt posY;
	UOSInt w;
	UOSInt h;
	this->parent->GetClientOfst(&lcliOfstX, &lcliOfstY);
	cliOfstX = Math::Double2Int32(lcliOfstX * this->hdpi / 96.0);
	cliOfstY = Math::Double2Int32(lcliOfstY * this->hdpi / 96.0);
	this->GetPositionP(&posX, &posY);
	this->GetSizeP(&w, &h);
	drawY = posY + y - this->dragY;
	if (drawY < dragMin)
	{
		drawY = dragMin;
	}
	else if (drawY > dragMax)
	{
		drawY = dragMax;
	}

	hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);
	hbr = CreatePatternBrush(hbm);

	SetBrushOrgEx((HDC)hdc, (int)(posX + cliOfstX), (int)(drawY + cliOfstY), 0);
	hbrushOld = (HBRUSH)SelectObject((HDC)hdc, hbr);

	PatBlt((HDC)hdc, (int)(posX + cliOfstX), (int)(drawY + cliOfstY), (int)w, (int)h, PATINVERT);
	
	SelectObject((HDC)hdc, hbrushOld);
	DeleteObject(hbr);
	DeleteObject(hbm);
}

void UI::GUIVSplitter::CalDragRange()
{
	OSInt i;
	OSInt max;
	OSInt min;
	UOSInt tmp;
	Bool foundTop = false;
	Bool foundBottom = false;
	Bool foundThis = false;
	UI::GUIControl *ctrl;
	UI::GUIControl::DockType dockType;
	min = 0;
	Double maxD;
	this->parent->GetClientSize(&maxD, 0);
	max = Math::Double2Int32(maxD * this->hdpi / 96.0);

	i = this->parent->GetChildCount();
	if (this->isBottom)
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
				if (dockType == UI::GUIControl::DOCK_BOTTOM)
				{
					if (foundThis && !foundBottom)
					{
						foundBottom = true;
						ctrl->GetPositionP(0, &max);
						ctrl->GetSizeP(0, &tmp);
						max += tmp;
					}
				}
				else if (dockType == UI::GUIControl::DOCK_TOP)
				{
					if (!foundTop)
					{
						foundTop = true;
						ctrl->GetPositionP(0, &min);
						ctrl->GetSizeP(0, &tmp);
						min += tmp;
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
				if (dockType == UI::GUIControl::DOCK_BOTTOM)
				{
					if (!foundBottom)
					{
						foundBottom = true;
						ctrl->GetPositionP(0, &max);
					}
				}
				else if (dockType == UI::GUIControl::DOCK_TOP)
				{
					if (foundThis && !foundTop)
					{
						foundTop = true;
						ctrl->GetPositionP(0, &min);
					}
				}
			}
		}
	}
	this->dragMax = max;
	this->dragMin = min;
}

UI::GUIVSplitter::GUIVSplitter(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 height, Bool isBottom) : UI::GUIControl(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui)->GetHInst());
	}

	Int32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)this->ui)->GetHInst(), parent, CLASSNAME, 0, style, 0, 0, 0, 10, height);
	this->SetDockType(isBottom?DOCK_BOTTOM:DOCK_TOP);
}

UI::GUIVSplitter::~GUIVSplitter()
{
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
}

const UTF8Char *UI::GUIVSplitter::GetObjectClass()
{
	return (const UTF8Char*)"VSplitter";
}

OSInt UI::GUIVSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
