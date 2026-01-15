#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUICustomDrawVScroll.h"
#include "UI/Win/WinCore.h"

#include <windows.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#define CLASSNAME L"CustomDrawVScroll"
Int32 UI::GUICustomDrawVScroll::useCnt = 0;

IntOS __stdcall UI::GUICustomDrawVScroll::CDVSWndProc(void *hWnd, UInt32 msg, UInt32 wParam, IntOS lParam)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)(IntOS)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
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
		me->OnMouseDown(GetScrollPos((HWND)hWnd, SB_VERT), Math::Coord2D<IntOS>(xPos, yPos), UI::GUIClientControl::MBTN_LEFT, keys);
		return 0;
	case WM_RBUTTONDOWN:
		me->Focus();
		yPos = (Int16)HIWORD(lParam);
		xPos = (Int16)LOWORD(lParam);
		keys = (KeyButton)wParam;
		me->OnMouseDown(GetScrollPos((HWND)hWnd, SB_VERT), Math::Coord2D<IntOS>(xPos, yPos), UI::GUIClientControl::MBTN_RIGHT, keys);
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
	BeginPaint((HWND)this->hwnd.OrNull(), &ps);
	SetBkMode(ps.hdc, TRANSPARENT);

	RECT rc;
	GetClientRect((HWND)this->hwnd.OrNull(), &rc);
	NN<Media::DrawImage> dimg;
	dimg = NN<Media::GDIEngine>::ConvertFrom(this->deng)->CreateImageScn(ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, this->colorSess);
	Double hdpi = this->GetHDPI();
	Double ddpi = this->GetDDPI();
	dimg->SetHDPI(hdpi / ddpi * 96.0);
	dimg->SetVDPI(hdpi / ddpi * 96.0);
	this->OnDraw(dimg);
	this->deng->DeleteImage(dimg);
	EndPaint((HWND)this->hwnd.OrNull(), &ps);
}

void UI::GUICustomDrawVScroll::Init(Optional<InstanceHandle> hInst)
{
	WNDCLASSW wc;
    wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)UI::GUICustomDrawVScroll::CDVSWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst.OrNull(); 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = 0; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUICustomDrawVScroll::Deinit(Optional<InstanceHandle> hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst.OrNull());
}

void UI::GUICustomDrawVScroll::ClearBackground(NN<Media::DrawImage> img)
{
	RECT rc;
	GetClientRect((HWND)this->hwnd.OrNull(), &rc);
	if (this->hbrBackground)
	{
		FillRect((HDC)((Media::GDIImage*)img.Ptr())->hdcBmp, &rc, (HBRUSH)this->hbrBackground);
	}
	else
	{
		FillRect((HDC)((Media::GDIImage*)img.Ptr())->hdcBmp, &rc, (HBRUSH)(COLOR_WINDOW));
	}
}

UI::GUICustomDrawVScroll::GUICustomDrawVScroll(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng, Optional<Media::ColorSess> colorSess) : UI::GUIControl(ui, parent)
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
	this->deng = deng;
	this->colorSess = colorSess;

	UInt32 style = WS_CHILD | WS_VSCROLL | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);
}

UI::GUICustomDrawVScroll::~GUICustomDrawVScroll()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}

Text::CStringNN UI::GUICustomDrawVScroll::GetObjectClass() const
{
	return CSTR("CustomDrawVScroll");
}

IntOS UI::GUICustomDrawVScroll::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUICustomDrawVScroll::OnSizeChanged(Bool updateScn)
{
	UIntOS i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUICustomDrawVScroll::OnMouseDown(IntOS scrollY, Math::Coord2D<IntOS> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
}

void UI::GUICustomDrawVScroll::OnKeyDown(UInt32 keyCode)
{
}

void UI::GUICustomDrawVScroll::HandleSelChg(UI::UIEvent hdlr, AnyType userObj)
{
	this->selChgHdlrs.Add({hdlr, userObj});
}

void UI::GUICustomDrawVScroll::HandleDblClk(UI::UIEvent hdlr, AnyType userObj)
{
	this->dblClkHdlrs.Add({hdlr, userObj});
}

void UI::GUICustomDrawVScroll::EventSelChg()
{
	UIntOS i;
	i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->selChgHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUICustomDrawVScroll::EventDblClk()
{
	UIntOS i;
	i = this->dblClkHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->dblClkHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUICustomDrawVScroll::SetVScrollBar(UIntOS min, UIntOS max, UIntOS pageSize)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.nMin = (int)(IntOS)min;
	si.nMax = (int)(IntOS)max;
	si.nPage = (UINT)pageSize;
	si.fMask = SIF_PAGE | SIF_RANGE;
	SetScrollInfo((HWND)this->hwnd.OrNull(), SB_VERT, &si, TRUE);
}

UIntOS UI::GUICustomDrawVScroll::GetVScrollPos()
{
	return (UInt32)GetScrollPos((HWND)this->hwnd.OrNull(), SB_VERT);
}

Bool UI::GUICustomDrawVScroll::MakeVisible(UIntOS firstIndex, UIntOS lastIndex)
{
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;
	GetScrollInfo((HWND)this->hwnd.OrNull(), SB_VERT, &si);

	if (lastIndex >= (UInt32)si.nMax)
	{
		lastIndex = (UInt32)si.nMax - 1;
	}
	if (firstIndex > lastIndex)
	{
		return false;
	}
	if (firstIndex < (UInt32)si.nMin)
		return false;

	if (si.nPos > (IntOS)firstIndex)
	{
		si.fMask = SIF_POS;
		si.nPos = (Int32)(IntOS)firstIndex;
		SetScrollInfo ((HWND)this->hwnd.OrNull(), SB_VERT, &si, TRUE);
		GetScrollInfo ((HWND)this->hwnd.OrNull(), SB_VERT, &si);
		this->Redraw();
		return true;
	}
	else if ((si.nPos + (Int32)si.nPage) > (IntOS)lastIndex)
	{
		return false;
	}
	else
	{
		si.fMask = SIF_POS;
		si.nPos = (Int32)(IntOS)lastIndex - (Int32)si.nPage + 1;
		SetScrollInfo ((HWND)this->hwnd.OrNull(), SB_VERT, &si, TRUE);
		GetScrollInfo ((HWND)this->hwnd.OrNull(), SB_VERT, &si);
		this->Redraw();
		return true;
	}
}
