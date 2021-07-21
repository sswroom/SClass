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
			si.nPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			si.nPos += si.nPage;
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

UI::GUIHScrollBar::GUIHScrollBar(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 width) : UI::GUIControl(ui, parent)
{
	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui)->GetHInst());
	}

	NEW_CLASS(this->posChgHdlrs, Data::ArrayList<PosChgEvent>());
	NEW_CLASS(this->posChgObjs, Data::ArrayList<void *>());

	Int32 style = WS_CHILD| WS_HSCROLL | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, CLASSNAME, 0, style, 0, 0, 0, width, GetSystemMetrics(SM_CYHSCROLL) + 1);
}

UI::GUIHScrollBar::~GUIHScrollBar()
{
	DEL_CLASS(this->posChgHdlrs);
	DEL_CLASS(this->posChgObjs);
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
}

void UI::GUIHScrollBar::InitScrollBar(OSInt minVal, OSInt maxVal, OSInt currVal, OSInt largeChg)
{
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	info.nMax = (int)maxVal;
	info.nMin = (int)minVal;
	info.nPage = (UINT)largeChg;
	info.nPos = (int)currVal;
	SetScrollInfo((HWND)this->hwnd, SB_HORZ, &info, TRUE);
	this->EventPosChanged();
}

void UI::GUIHScrollBar::SetPos(OSInt pos)
{
	SetScrollPos((HWND)this->hwnd, SB_HORZ, (int)pos, TRUE);
	this->EventPosChanged();
}

OSInt UI::GUIHScrollBar::GetPos()
{
	return GetScrollPos((HWND)this->hwnd, SB_HORZ);
}

void UI::GUIHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left;
	this->lyPos = top;
	Int32 minSize = GetSystemMetrics(SM_CYHSCROLL) + 1;
	Int32 newSize = Math::Double2Int32((bottom - top) * this->hdpi / this->ddpi);
	if (newSize < minSize)
		newSize = minSize;
	this->selfResize = true;
	MoveWindow((HWND)hwnd, Math::Double2Int32((left + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((top + yOfst) * this->hdpi / this->ddpi), Math::Double2Int32((right - left) * this->hdpi / this->ddpi), newSize, updateScn?TRUE:FALSE);
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	this->lxPos2 = left + (rect.right - rect.left) * this->ddpi / this->hdpi;
	this->lyPos2 = top + (rect.bottom - rect.top) * this->ddpi / this->hdpi;
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIHScrollBar::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left * this->ddpi / this->hdpi;
	this->lyPos = top * this->ddpi / this->hdpi;
	Int32 minSize = GetSystemMetrics(SM_CYHSCROLL) + 1;
	Int32 newSize = (Int32)(bottom - top);
	if (newSize < minSize)
		newSize = minSize;
	this->selfResize = true;
	MoveWindow((HWND)hwnd, Math::Double2Int32(left + xOfst * this->hdpi / this->ddpi), Math::Double2Int32(top + yOfst * this->hdpi / this->ddpi), (int)(right - left), newSize, updateScn?TRUE:FALSE);
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	this->lxPos2 = (left + rect.right - rect.left) * this->ddpi / this->hdpi;
	this->lyPos2 = (top + rect.bottom - rect.top) * this->ddpi / this->hdpi;
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

const UTF8Char *UI::GUIHScrollBar::GetObjectClass()
{
	return (const UTF8Char*)"HScrollBar";
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

	if (this->parent)
	{
		Double xOfst = 0;
		Double yOfst = 0;
		this->parent->GetClientOfst(&xOfst, &yOfst);
		newSize = Math::Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)hwnd, Math::Double2Int32((this->lxPos + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((this->lyPos + yOfst) * this->hdpi / this->ddpi), Math::Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), newSize, redraw?TRUE:FALSE);
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
		newSize = Math::Double2Int32(newH);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)this->hwnd, Math::Double2Int32(newX), Math::Double2Int32(newY), Math::Double2Int32(newW), newSize, redraw?TRUE:FALSE);
	}
}

void UI::GUIHScrollBar::EventPosChanged()
{
	Int32 newPos = GetScrollPos((HWND)this->hwnd, SB_HORZ);
	UOSInt i;
	i = this->posChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->posChgHdlrs->GetItem(i)(this->posChgObjs->GetItem(i), newPos);
	}
}

void UI::GUIHScrollBar::HandlePosChanged(PosChgEvent hdlr, void *userObj)
{
	this->posChgHdlrs->Add(hdlr);
	this->posChgObjs->Add(userObj);
}

Int32 UI::GUIHScrollBar::GetSystemSize()
{
	return GetSystemMetrics(SM_CYHSCROLL);
}
