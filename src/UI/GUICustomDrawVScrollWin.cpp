#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUICoreWin.h"
#include "UI/GUICustomDrawVScroll.h"

#include <windows.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#define CLASSNAME L"CustomDrawVScroll"
Int32 UI::GUICustomDrawVScroll::useCnt = 0;

OSInt __stdcall UI::GUICustomDrawVScroll::CDVSWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	SCROLLINFO si;
	Int32 xPos;
	Int32 yPos;
	KeyButton keys;
	switch (msg)
	{
	case WM_SIZE:
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_VSCROLL:
		si.cbSize = sizeof (si);
		si.fMask  = SIF_ALL;

		GetScrollInfo((HWND)hWnd, SB_VERT, &si);
		xPos = si.nPos;
		switch (LOWORD (wParam))
		{
		case SB_LINELEFT: 
			si.nPos -= 1;
			break;
		case SB_LINERIGHT: 
			si.nPos += 1;
			break;
		case SB_PAGELEFT:
			si.nPos -= (INT)si.nPage;
			break;
		case SB_PAGERIGHT:
			si.nPos += (INT)si.nPage;
			break;
		case SB_THUMBTRACK: 
			si.nPos = si.nTrackPos;
			break;
		default :
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo ((HWND)hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo ((HWND)hWnd, SB_VERT, &si);

		me->Redraw();
		return 0;
	case WM_LBUTTONDOWN:
		me->Focus();
		yPos = (Int16)HIWORD(lParam);
		xPos = (Int16)LOWORD(lParam);
		keys = (KeyButton)wParam;
		me->OnMouseDown(GetScrollPos((HWND)hWnd, SB_VERT), xPos, yPos, UI::GUIClientControl::MBTN_LEFT, keys);
		return 0;
	case WM_RBUTTONDOWN:
		me->Focus();
		yPos = (Int16)HIWORD(lParam);
		xPos = (Int16)LOWORD(lParam);
		keys = (KeyButton)wParam;
		me->OnMouseDown(GetScrollPos((HWND)hWnd, SB_VERT), xPos, yPos, UI::GUIClientControl::MBTN_RIGHT, keys);
		return 0;
	case WM_LBUTTONDBLCLK:
		me->EventDblClk();
		break;
	case WM_PAINT:
		me->OnPaint();
		return 0;
	case WM_MOUSEWHEEL:
		{
			Int32 delta = (Int16)HIWORD(wParam);
			si.cbSize = sizeof (si);
			si.fMask  = SIF_ALL;

			GetScrollInfo((HWND)hWnd, SB_VERT, &si);
			xPos = (Int32)si.nPage >> 2;
			if (xPos <= 0)
			{
				xPos = 1;
			}
			if (delta > 0)
			{
				si.nPos -= xPos;
			}
			else if (delta < 0)
			{
				si.nPos += xPos;
			}
			si.fMask = SIF_POS;
			SetScrollInfo ((HWND)hWnd, SB_VERT, &si, TRUE);
			GetScrollInfo ((HWND)hWnd, SB_VERT, &si);

			me->Redraw();
		}
		return 0;
	case WM_KEYDOWN:
		me->OnKeyDown(wParam);
		break;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUICustomDrawVScroll::OnPaint()
{
	PAINTSTRUCT ps;
	BeginPaint((HWND)this->hwnd, &ps);
	SetBkMode(ps.hdc, TRANSPARENT);

	RECT rc;
	GetClientRect((HWND)this->hwnd, &rc);
	Media::DrawImage *dimg = ((Media::GDIEngine*)this->deng)->CreateImageScn(ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top);
	Double hdpi = this->GetHDPI();
	Double ddpi = this->GetDDPI();
	dimg->SetHDPI(hdpi / ddpi * 96.0);
	dimg->SetVDPI(hdpi / ddpi * 96.0);
	this->OnDraw(dimg);
	this->deng->DeleteImage(dimg);

	EndPaint((HWND)this->hwnd, &ps);
}

void UI::GUICustomDrawVScroll::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)UI::GUICustomDrawVScroll::CDVSWndProc; 
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

void UI::GUICustomDrawVScroll::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::GUICustomDrawVScroll::ClearBackground(Media::DrawImage *img)
{
	RECT rc;
	GetClientRect((HWND)this->hwnd, &rc);
	if (this->hbrBackground)
	{
		FillRect((HDC)((Media::GDIImage*)img)->hdcBmp, &rc, (HBRUSH)this->hbrBackground);
	}
	else
	{
		FillRect((HDC)((Media::GDIImage*)img)->hdcBmp, &rc, (HBRUSH)(COLOR_WINDOW));
	}
}

UI::GUICustomDrawVScroll::GUICustomDrawVScroll(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng) : UI::GUIControl(ui, parent)
{
	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
	this->deng = deng;

	UInt32 style = WS_CHILD | WS_VSCROLL | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);

	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->dblClkHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->dblClkObjs, Data::ArrayList<void *>());
}

UI::GUICustomDrawVScroll::~GUICustomDrawVScroll()
{
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
	DEL_CLASS(this->dblClkHdlrs);
	DEL_CLASS(this->dblClkObjs);
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
}

Text::CString UI::GUICustomDrawVScroll::GetObjectClass()
{
	return CSTR("CustomDrawVScroll");
}

OSInt UI::GUICustomDrawVScroll::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUICustomDrawVScroll::OnSizeChanged(Bool updateScn)
{
	UOSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

void UI::GUICustomDrawVScroll::OnMouseDown(OSInt scrollY, Int32 xPos, Int32 yPos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
}

void UI::GUICustomDrawVScroll::OnKeyDown(UInt32 keyCode)
{
}

void UI::GUICustomDrawVScroll::HandleSelChg(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUICustomDrawVScroll::HandleDblClk(UI::UIEvent hdlr, void *userObj)
{
	this->dblClkHdlrs->Add(hdlr);
	this->dblClkObjs->Add(userObj);
}

void UI::GUICustomDrawVScroll::EventSelChg()
{
	UOSInt i;
	i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUICustomDrawVScroll::EventDblClk()
{
	UOSInt i;
	i = this->dblClkHdlrs->GetCount();
	while (i-- > 0)
	{
		this->dblClkHdlrs->GetItem(i)(this->dblClkObjs->GetItem(i));
	}
}

void UI::GUICustomDrawVScroll::SetVScrollBar(UOSInt min, UOSInt max, UOSInt pageSize)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.nMin = (int)(OSInt)min;
	si.nMax = (int)(OSInt)max;
	si.nPage = (UINT)pageSize;
	si.fMask = SIF_PAGE | SIF_RANGE;
	SetScrollInfo((HWND)this->hwnd, SB_VERT, &si, TRUE);
}

UOSInt UI::GUICustomDrawVScroll::GetVScrollPos()
{
	return (UInt32)GetScrollPos((HWND)this->hwnd, SB_VERT);
}

Bool UI::GUICustomDrawVScroll::MakeVisible(UOSInt index)
{
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;
	GetScrollInfo((HWND)this->hwnd, SB_VERT, &si);

	if (index < (UInt32)si.nMin)
		return false;
	if (index >= (UInt32)si.nMax)
		return false;

	if (si.nPos > (OSInt)index)
	{
		si.fMask = SIF_POS;
		si.nPos = (Int32)(OSInt)index;
		SetScrollInfo ((HWND)this->hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo ((HWND)this->hwnd, SB_VERT, &si);
		this->Redraw();
		return true;
	}
	else if ((si.nPos + (Int32)si.nPage) > (OSInt)index)
	{
		return false;
	}
	else
	{
		si.fMask = SIF_POS;
		si.nPos = (Int32)(OSInt)index - (Int32)si.nPage + 1;
		SetScrollInfo ((HWND)this->hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo ((HWND)this->hwnd, SB_VERT, &si);
		this->Redraw();
		return true;
	}
}
