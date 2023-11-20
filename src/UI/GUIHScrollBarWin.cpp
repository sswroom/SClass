#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIHScrollBar.h"
#include <windows.h>

#define CLASSNAME L"SSWRSBAR"
Int32 UI::GUIHScrollBar::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUIHScrollBar::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIHScrollBar *me = (UI::GUIHScrollBar*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	SCROLLINFO si;
	Bool noChg = false;
	switch (msg)
	{
	case WM_HSCROLL:
		si.cbSize = sizeof (si);
		si.fMask  = SIF_ALL;

		GetScrollInfo((HWND)hWnd, SB_HORZ, &si);
		switch (LOWORD (wParam))
		{
		case SB_LINELEFT: 
			si.nPos -= 1;
			break;
		case SB_LINERIGHT: 
			si.nPos += 1;
			break;
		case SB_PAGELEFT:
			si.nPos -= (int)si.nPage;
			break;
		case SB_PAGERIGHT:
			si.nPos += (int)si.nPage;
			break;
		case SB_THUMBTRACK: 
			si.nPos = si.nTrackPos;
			break;
		default :
			noChg = true;
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo ((HWND)hWnd, SB_HORZ, &si, TRUE);
		GetScrollInfo ((HWND)hWnd, SB_HORZ, &si);

		if (!noChg)
		{
			me->EventPosChanged();
		}
		return 0;

	default:
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

void UI::GUIHScrollBar::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIHScrollBar::FormWndProc; 
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

void UI::GUIHScrollBar::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

UI::GUIHScrollBar::GUIHScrollBar(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 width) : UI::GUIControl(ui, parent)
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CHILD| WS_HSCROLL | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui.Ptr())->GetHInst(), parent, CLASSNAME, 0, style, 0, 0, 0, width, GetSystemMetrics(SM_CYHSCROLL) + 1);
}

UI::GUIHScrollBar::~GUIHScrollBar()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst());
	}
}

void UI::GUIHScrollBar::InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg)
{
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	info.nMax = (int)(UInt32)maxVal;
	info.nMin = (int)(UInt32)minVal;
	info.nPage = (UINT)largeChg;
	info.nPos = (int)(UInt32)currVal;
	SetScrollInfo((HWND)this->hwnd, SB_HORZ, &info, TRUE);
	this->EventPosChanged();
}

void UI::GUIHScrollBar::SetPos(UOSInt pos)
{
	SetScrollPos((HWND)this->hwnd, SB_HORZ, (int)(OSInt)pos, TRUE);
	this->EventPosChanged();
}

UOSInt UI::GUIHScrollBar::GetPos()
{
	return (UOSInt)(OSInt)GetScrollPos((HWND)this->hwnd, SB_HORZ);
}

void UI::GUIHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	this->lxPos = left;
	this->lyPos = top;
	Int32 minSize = GetSystemMetrics(SM_CYHSCROLL) + 1;
	Int32 newSize = Double2Int32((bottom - top) * this->hdpi / this->ddpi);
	if (newSize < minSize)
		newSize = minSize;
	this->selfResize = true;
	MoveWindow((HWND)hwnd, Double2Int32((left + ofst.x) * this->hdpi / this->ddpi), Double2Int32((top + ofst.y) * this->hdpi / this->ddpi), Double2Int32((right - left) * this->hdpi / this->ddpi), newSize, updateScn?TRUE:FALSE);
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	this->lxPos2 = left + (rect.right - rect.left) * this->ddpi / this->hdpi;
	this->lyPos2 = top + (rect.bottom - rect.top) * this->ddpi / this->hdpi;
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIHScrollBar::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	this->lxPos = OSInt2Double(left) * this->ddpi / this->hdpi;
	this->lyPos = OSInt2Double(top) * this->ddpi / this->hdpi;
	Int32 minSize = GetSystemMetrics(SM_CYHSCROLL) + 1;
	Int32 newSize = (Int32)(bottom - top);
	if (newSize < minSize)
		newSize = minSize;
	this->selfResize = true;
	MoveWindow((HWND)hwnd, Double2Int32(OSInt2Double(left) + ofst.x * this->hdpi / this->ddpi), Double2Int32(OSInt2Double(top) + ofst.y * this->hdpi / this->ddpi), (int)(right - left), newSize, updateScn?TRUE:FALSE);
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	this->lxPos2 = OSInt2Double(left + rect.right - rect.left) * this->ddpi / this->hdpi;
	this->lyPos2 = OSInt2Double(top + rect.bottom - rect.top) * this->ddpi / this->hdpi;
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

Text::CStringNN UI::GUIHScrollBar::GetObjectClass() const
{
	return CSTR("HScrollBar");
}

OSInt UI::GUIHScrollBar::OnNotify(UInt32 code, void *lParam)
{
	if (code == WM_HSCROLL)
	{
	}
	return 0;
}

void UI::GUIHScrollBar::UpdatePos(Bool redraw)
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	if (GetWindowInfo((HWND)hwnd, &wi))
	{
		if (wi.dwStyle & (WS_MAXIMIZE | WS_MINIMIZE))
		{
			return;
		}
	}
	Int32 minSize = GetSystemMetrics(SM_CYHSCROLL) + 1;
	Int32 newSize;

	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		Math::Coord2DDbl ofst = nnparent->GetClientOfst();
		newSize = Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)hwnd, Double2Int32((this->lxPos + ofst.x) * this->hdpi / this->ddpi), Double2Int32((this->lyPos + ofst.y) * this->hdpi / this->ddpi), Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), newSize, redraw?TRUE:FALSE);
	}
	else
	{
		Double newW = (this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi;
		Double newH = (this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi;
		int maxX = GetSystemMetrics(SM_CXSCREEN);
		int maxY = GetSystemMetrics(SM_CYSCREEN);
		if (newW > maxX)
			newW = maxX;
		if (newH > maxY)
			newH = maxY;
		RECT rc;
		GetWindowRect((HWND)this->hwnd, &rc);
		Double newX = (rc.left + rc.right - newW) * 0.5;
		Double newY = (rc.top + rc.bottom - newH) * 0.5;
		if (newY < 0)
		{
			newY = 0;
		}
		newSize = Double2Int32(newH);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)this->hwnd, Double2Int32(newX), Double2Int32(newY), Double2Int32(newW), newSize, redraw?TRUE:FALSE);
	}
}

void UI::GUIHScrollBar::EventPosChanged()
{
	UOSInt newPos = this->GetPos();
	UOSInt i;
	i = this->posChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->posChgHdlrs.GetItem(i)(this->posChgObjs.GetItem(i), newPos);
	}
}

void UI::GUIHScrollBar::HandlePosChanged(PosChgEvent hdlr, void *userObj)
{
	this->posChgHdlrs.Add(hdlr);
	this->posChgObjs.Add(userObj);
}

Int32 UI::GUIHScrollBar::GetSystemSize()
{
	return GetSystemMetrics(SM_CYHSCROLL);
}
