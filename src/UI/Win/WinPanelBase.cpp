#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinPanelBase.h"
#include <windows.h>

#define CLASSNAME L"Panel"
#define BGBRUSH ((HBRUSH)COLOR_BTNSHADOW)
Int32 UI::Win::WinPanelBase::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#if defined(_WIN32_WCE)
#define GetWindowLongPtr(a, b) GetWindowLongW(a, b)
#endif

OSInt __stdcall UI::Win::WinPanelBase::PnlWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIPanel *pnl = (UI::GUIPanel*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	if (pnl == 0)
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	NN<UI::Win::WinPanelBase> me = NN<UI::Win::WinPanelBase>::ConvertFrom(pnl->GetBase());
	UI::GUIControl *ctrl;
	SCROLLINFO si;
	Bool upd;
	NMHDR *nmhdr;
	switch (msg)
	{
	case WM_HSCROLL:
		if (me->scrollH)
		{
			si.cbSize = sizeof (si);
			si.fMask  = SIF_ALL;

			GetScrollInfo((HWND)hWnd, SB_HORZ, &si);
			upd = false;
			if (me->currScrX != si.nPos)
			{
				me->currScrX = si.nPos;
				upd = true;
			}
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
				break;
			}
			si.fMask = SIF_POS;
			SetScrollInfo ((HWND)hWnd, SB_HORZ, &si, TRUE);
			GetScrollInfo ((HWND)hWnd, SB_HORZ, &si);
			if (upd)
			{
				me->master->UpdateChildrenSize(true);
			}
		}
		return 0;
	case WM_VSCROLL:
		if (me->scrollV)
		{
			si.cbSize = sizeof (si);
			si.fMask  = SIF_ALL;

			GetScrollInfo((HWND)hWnd, SB_VERT, &si);
			upd = false;
			if (me->currScrY != si.nPos)
			{
				me->currScrY = si.nPos;
				upd = true;
			}
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
				break;
			}
			si.fMask = SIF_POS;
			SetScrollInfo ((HWND)hWnd, SB_VERT, &si, TRUE);
			GetScrollInfo ((HWND)hWnd, SB_VERT, &si);
			if (upd)
			{
				me->master->UpdateChildrenSize(true);
			}
		}
		return 0;
	case WM_COMMAND:
		ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
		if (ctrl)
		{
			ctrl->OnNotify(HIWORD(wParam), 0);
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr(nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			ctrl->OnNotify(nmhdr->code, (void*)lParam);
		}
		break;
	case WM_SIZE:
		me->master->OnSizeChanged(false);
		me->UpdateScrollBars();
		me->master->UpdateChildrenSize(false);
/*			if (me->visible)
		{
			InvalidateRect((HWND)hWnd, 0, false);
		}*/
		return 0;//DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	case WM_ERASEBKGND:
		return me->master->MyEraseBkg((void*)wParam);
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC) wParam;
			HWND hwndStatic = (HWND)lParam;
			UI::GUIControl *ctrl = (UI::GUIControl*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hwndStatic, GWL_USERDATA);
			if (ctrl == 0)
			{
			}
			else
			{
				void *b = ctrl->GetBGBrush();
				if (ctrl->GetObjectClass().Equals(UTF8STRC("Label")))
				{
					UI::GUILabel *lbl = (UI::GUILabel*)ctrl;
					if (lbl->HasTextColor())
					{
						UInt32 c = lbl->GetTextColor();
						SetTextColor(hdcStatic, ((c & 0xff) << 16) | (c & 0xff00) | ((c & 0xff0000) >> 16));
						if (b == 0)
						{
							SetBkMode(hdcStatic, TRANSPARENT);
							return (OSInt)GetSysColorBrush(COLOR_BTNFACE);
						}
					}
				}
				if (b)
				{
					SetBkMode(hdcStatic, TRANSPARENT);
					return (OSInt)ctrl->GetBGBrush();//me->GetBGBrush();
				}
			}
		}
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::Win::WinPanelBase::Init(Optional<InstanceHandle> hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::Win::WinPanelBase::PnlWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst.OrNull(); 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = BGBRUSH; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::Win::WinPanelBase::Deinit(Optional<InstanceHandle> hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst.OrNull());
}

void UI::Win::WinPanelBase::UpdateScrollBars()
{
	RECT rc;
	Int32 w;
	Int32 h;
	Int32 vsSize = GetSystemMetrics(SM_CXVSCROLL);
	Int32 hsSize = GetSystemMetrics(SM_CYHSCROLL);
	SCROLLINFO info;
	GetClientRect((HWND)this->master->GetHandle().OrNull(), &rc);
	w = rc.right - rc.left;
	h = rc.bottom - rc.top;
	if (w >= this->minW && h >= this->minH)
	{
		this->currScrX = 0;
		this->currScrY = 0;
		if (this->scrollH)
		{
			this->scrollH = false;
		}
		if (this->scrollV)
		{
			this->scrollV = false;
		}
#ifndef _WIN32_WCE
		ShowScrollBar((HWND)this->master->GetHandle().OrNull(), SB_BOTH, FALSE);
#endif
	}
	else if (w < this->minW)
	{
		if (!this->scrollH)
		{
#ifndef _WIN32_WCE
			ShowScrollBar((HWND)this->master->GetHandle().OrNull(), SB_HORZ, TRUE);
#endif
			this->scrollH = true;
		}
		if (h - hsSize < this->minH)
		{
			if (!this->scrollV)
			{
#ifndef _WIN32_WCE
				ShowScrollBar((HWND)this->master->GetHandle().OrNull(), SB_VERT, TRUE);
#endif
				this->scrollV = true;
			}
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_PAGE | SIF_RANGE; //SIF_POS
			info.nMax = this->minH;
			info.nMin = 0;
			info.nPage = (UInt32)(h - hsSize);
//			info.nPos = this->currScrY;
			SetScrollInfo((HWND)this->master->GetHandle().OrNull(), SB_VERT, &info, TRUE);
			w -= vsSize;
		}
		else if (this->scrollV)
		{
#ifndef _WIN32_WCE
			ShowScrollBar((HWND)this->master->GetHandle().OrNull(), SB_VERT, FALSE);
#endif
			this->scrollV = false;
			this->currScrY = 0;
		}
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_PAGE | SIF_RANGE; // | SIF_POS
		info.nMax = this->minW;
		info.nMin = 0;
		info.nPage = (UInt32)w;
//		info.nPos = this->currScrX;
		SetScrollInfo((HWND)this->master->GetHandle().OrNull(), SB_HORZ, &info, TRUE);
	}
	else if (h < this->minH)
	{
		if (!this->scrollV)
		{
#ifndef _WIN32_WCE
			ShowScrollBar((HWND)this->master->GetHandle().OrNull(), SB_VERT, TRUE);
#endif
			this->scrollV = true;
		}
		if (w - vsSize < this->minW)
		{
			if (!this->scrollH)
			{
#ifndef _WIN32_WCE
				ShowScrollBar((HWND)this->master->GetHandle().OrNull(), SB_HORZ, TRUE);
#endif
				this->scrollH = true;
			}
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_PAGE | SIF_RANGE; //SIF_POS
			info.nMax = this->minW;
			info.nMin = 0;
			info.nPage = (UInt32)(w - vsSize);
//			info.nPos = this->currScrX;
			SetScrollInfo((HWND)this->master->GetHandle().OrNull(), SB_HORZ, &info, TRUE);
			h -= hsSize;
		}
		else if (this->scrollH)
		{
#ifndef _WIN32_WCE
			ShowScrollBar((HWND)this->master->GetHandle().OrNull(), SB_HORZ, FALSE);
#endif
			this->scrollH = false;
			this->currScrX = 0;
		}
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_PAGE | SIF_RANGE; // | SIF_POS
		info.nMax = this->minH;
		info.nMin = 0;
		info.nPage = (UInt32)h;
//		info.nPos = this->currScrY;
		SetScrollInfo((HWND)this->master->GetHandle().OrNull(), SB_VERT, &info, TRUE);
	}
}

UI::Win::WinPanelBase::WinPanelBase(NN<GUIPanel> master, NN<UI::GUICore> ui, Optional<ControlHandle> parentHWnd)
{
	this->master = master;
	this->ui = NN<WinCore>::ConvertFrom(ui);
	this->minW = 0;
	this->minH = 0;
	this->scrollH = false;
	this->scrollV = false;
	this->currScrX = 0;
	this->currScrY = 0;
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(this->ui->GetHInst());
	}

	UInt32 style = WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE;
#ifdef _WIN32_WCE
	this->master->InitControlHand(this->ui->GetHInst(), parentHWnd, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);
#else
	this->master->InitControlHand(this->ui->GetHInst(), parentHWnd, CLASSNAME, (const UTF8Char*)"", style, WS_EX_CONTROLPARENT, 0, 0, 200, 200);
#endif
}

UI::Win::WinPanelBase::WinPanelBase(NN<GUIPanel> master, NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent)
{
	this->master = master;
	this->ui = NN<WinCore>::ConvertFrom(ui);
	this->minW = 0;
	this->minH = 0;
	this->scrollH = false;
	this->scrollV = false;
	this->currScrX = 0;
	this->currScrY = 0;
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(this->ui->GetHInst());
	}

	UInt32 style = WS_CLIPSIBLINGS | WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
#ifdef _WIN32_WCE
	this->master->InitControl(this->ui->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);
#else
	this->master->InitControl(this->ui->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, WS_EX_CONTROLPARENT, 0, 0, 200, 200);
#endif
}

UI::Win::WinPanelBase::~WinPanelBase()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(this->ui->GetHInst());
	}
}

Math::Coord2DDbl UI::Win::WinPanelBase::GetClientOfst()
{
	return Math::Coord2DDbl(-this->currScrX, -this->currScrY);
}

Math::Size2DDbl UI::Win::WinPanelBase::GetClientSize()
{
	Int32 cliW;
	Int32 cliH;
	RECT rc;
	GetClientRect((HWND)this->master->GetHandle().OrNull(), &rc);
	cliW = rc.right - rc.left;
	cliH = rc.bottom - rc.top;
	if (cliW < this->minW)
		cliW = this->minW;
	if (cliH < this->minH)
		cliH = this->minH;
	return Math::Size2DDbl(cliW, cliH) * this->master->GetDDPI() / this->master->GetHDPI();
}

void UI::Win::WinPanelBase::SetMinSize(Int32 minW, Int32 minH)
{
	this->minW = minW;
	this->minH = minH;
	this->UpdateScrollBars();
}
