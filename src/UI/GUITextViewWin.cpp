#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "UI/GUICoreWin.h"
#include "UI/GUITextView.h"
#include <windows.h>

#define READBUFFSIZE 1048576
#define CLASSNAME L"TextView"
#define BGBRUSH ((HBRUSH)COLOR_WINDOWFRAME)
#define HK_PAGEUP 1
#define HK_PAGEDOWN 2
#define HK_LINEUP 3
#define HK_LINEDOWN 4
#define HK_HOME 5
#define HK_END 6
#define HK_COPY 7
#define HK_COPY_OLD 8
#define HK_LEFT 9
#define HK_RIGHT 10
#define HK_LINEHOME 11
#define HK_LINEEND 12

#define HK_SPAGEUP 13
#define HK_SPAGEDOWN 14
#define HK_SLINEUP 15
#define HK_SLINEDOWN 16
#define HK_SHOME 17
#define HK_SEND 18
#define HK_SLEFT 19
#define HK_SRIGHT 20
#define HK_SLINEHOME 21
#define HK_SLINEEND 22

OSInt UI::GUITextView::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUITextView::TFVWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::GUITextView *me = (UI::GUITextView*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	UI::GUIControl*ctrl;
	NMHDR *nmhdr;
	SCROLLINFO si;
	Int32 mouseXPos;
	Int32 mouseYPos;
	Int16 scrollLock;
	switch (msg)
	{
	case WM_COMMAND:
		ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(HIWORD(wParam), 0);
		}
		break;
	case WM_KILLFOCUS:
		UnregisterHotKey((HWND)hWnd, HK_PAGEUP);
		UnregisterHotKey((HWND)hWnd, HK_PAGEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_LINEUP);
		UnregisterHotKey((HWND)hWnd, HK_LINEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_HOME);
		UnregisterHotKey((HWND)hWnd, HK_END);
		UnregisterHotKey((HWND)hWnd, HK_COPY);
		UnregisterHotKey((HWND)hWnd, HK_COPY_OLD);
		UnregisterHotKey((HWND)hWnd, HK_LEFT);
		UnregisterHotKey((HWND)hWnd, HK_RIGHT);
		UnregisterHotKey((HWND)hWnd, HK_LINEHOME);
		UnregisterHotKey((HWND)hWnd, HK_LINEEND);

		UnregisterHotKey((HWND)hWnd, HK_SPAGEUP);
		UnregisterHotKey((HWND)hWnd, HK_SPAGEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_SLINEUP);
		UnregisterHotKey((HWND)hWnd, HK_SLINEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_SHOME);
		UnregisterHotKey((HWND)hWnd, HK_SEND);
		UnregisterHotKey((HWND)hWnd, HK_SLEFT);
		UnregisterHotKey((HWND)hWnd, HK_SRIGHT);
		UnregisterHotKey((HWND)hWnd, HK_SLINEHOME);
		UnregisterHotKey((HWND)hWnd, HK_SLINEEND);
		DestroyCaret(); 
		break;
	case WM_SETFOCUS:
		RegisterHotKey((HWND)hWnd, HK_PAGEUP, 0, VK_PRIOR);
		RegisterHotKey((HWND)hWnd, HK_PAGEDOWN, 0, VK_NEXT);
		RegisterHotKey((HWND)hWnd, HK_LINEUP, 0, VK_UP);
		RegisterHotKey((HWND)hWnd, HK_LINEDOWN, 0, VK_DOWN);
		RegisterHotKey((HWND)hWnd, HK_HOME, MOD_CONTROL, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_END, MOD_CONTROL, VK_END);
		RegisterHotKey((HWND)hWnd, HK_COPY_OLD, MOD_CONTROL, VK_INSERT);
		RegisterHotKey((HWND)hWnd, HK_COPY, MOD_CONTROL, 0x43);
		RegisterHotKey((HWND)hWnd, HK_LEFT, 0, VK_LEFT);
		RegisterHotKey((HWND)hWnd, HK_RIGHT, 0, VK_RIGHT);
		RegisterHotKey((HWND)hWnd, HK_LINEHOME, 0, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_LINEEND, 0, VK_END);

		RegisterHotKey((HWND)hWnd, HK_SPAGEUP, MOD_SHIFT, VK_PRIOR);
		RegisterHotKey((HWND)hWnd, HK_SPAGEDOWN, MOD_SHIFT, VK_NEXT);
		RegisterHotKey((HWND)hWnd, HK_SLINEUP, MOD_SHIFT, VK_UP);
		RegisterHotKey((HWND)hWnd, HK_SLINEDOWN, MOD_SHIFT, VK_DOWN);
		RegisterHotKey((HWND)hWnd, HK_SHOME, MOD_CONTROL | MOD_SHIFT, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_SEND, MOD_CONTROL | MOD_SHIFT, VK_END);
		RegisterHotKey((HWND)hWnd, HK_SLEFT, MOD_SHIFT, VK_LEFT);
		RegisterHotKey((HWND)hWnd, HK_SRIGHT, MOD_SHIFT, VK_RIGHT);
		RegisterHotKey((HWND)hWnd, HK_SLINEHOME, MOD_SHIFT, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_SLINEEND, MOD_SHIFT, VK_END);

		CreateCaret((HWND)hWnd, 0, 2, Double2Int32(me->pageLineHeight));
		me->UpdateCaretPos();
		ShowCaret((HWND)hWnd); 
		break;
	case WM_HOTKEY:
		switch (wParam)
		{
		case HK_PAGEUP:
		case HK_SPAGEUP:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) - (Int32)me->pageLineCnt, TRUE);
				me->Redraw();
			}
			else
			{
				me->EventPageUp();
			}
			break;
		case HK_PAGEDOWN:
		case HK_SPAGEDOWN:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) + (Int32)me->pageLineCnt, TRUE);
				me->Redraw();
			}
			else
			{
				me->EventPageDown();
			}
			break;
		case HK_LINEUP:
		case HK_SLINEUP:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) - 1, TRUE);
				me->Redraw();
			}
			else
			{
				me->EventLineUp();
			}
			break;
		case HK_LINEDOWN:
		case HK_SLINEDOWN:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) + 1, TRUE);
				me->Redraw();
			}
			else
			{
				me->EventLineDown();
			}
			break;
		case HK_LEFT:
		case HK_SLEFT:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_HORZ, GetScrollPos((HWND)hWnd, SB_HORZ) - 1, TRUE);
				me->Redraw();
			}
			else
			{
				me->EventLeft();
			}
			break;
		case HK_RIGHT:
		case HK_SRIGHT:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_HORZ, GetScrollPos((HWND)hWnd, SB_HORZ) + 1, TRUE);
				me->Redraw();
			}
			else
			{
				me->EventRight();
			}
			break;
		case HK_HOME:
		case HK_SHOME:
			me->EventHome();
			break;
		case HK_END:
		case HK_SEND:
			me->EventEnd();
			break;
		case HK_COPY:
		case HK_COPY_OLD:
			me->EventCopy();
			break;
		case HK_LINEHOME:
		case HK_SLINEHOME:
			me->EventLineBegin();
			break;
		case HK_LINEEND:
		case HK_SLINEEND:
			me->EventLineEnd();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		mouseXPos = (Int16)LOWORD(lParam);
		mouseYPos = (Int16)HIWORD(lParam);
		SetFocus((HWND)hWnd);
		me->EventMouseDown(mouseXPos, mouseYPos, MBTN_LEFT);
		::SetCapture((HWND)hWnd);
		break;
	case WM_MOUSEMOVE:
		mouseXPos = (Int16)LOWORD(lParam);
		mouseYPos = (Int16)HIWORD(lParam);
		me->EventMouseMove(mouseXPos, mouseYPos);
		break;
	case WM_LBUTTONUP:
		mouseXPos = (Int16)LOWORD(lParam);
		mouseYPos = (Int16)HIWORD(lParam);
		me->EventMouseUp(mouseXPos, mouseYPos, MBTN_LEFT);
		::ReleaseCapture();
		break;
	case WM_MOUSEWHEEL:
		{
			UInt32 ucNumLines = 0;
			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ucNumLines, 0);
			if (ucNumLines == 0)
				ucNumLines = 3;
			SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) - (Int16)(HIWORD(wParam)) / 120 * (Int32)ucNumLines, TRUE);
			me->Redraw();
		}
		break;
	case WM_HSCROLL:
		si.cbSize = sizeof (si);
		si.fMask  = SIF_ALL;

		GetScrollInfo((HWND)hWnd, SB_HORZ, &si);
		mouseXPos = si.nPos;
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

		if (si.nPos != mouseXPos)
		{
			me->Redraw();
		}
		return 0;
	case WM_VSCROLL:
		si.cbSize = sizeof (si);
		si.fMask  = SIF_ALL;
		GetScrollInfo ((HWND)hWnd, SB_VERT, &si);

		mouseYPos = si.nPos;
		switch (LOWORD (wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			si.nPos += 1;
			break;
		case SB_PAGEUP:
			si.nPos -= (int)si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += (int)si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		default:
			break; 
		}
		si.fMask = SIF_POS;
		SetScrollInfo ((HWND)hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo ((HWND)hWnd, SB_VERT, &si);

		if (si.nPos != mouseYPos)
		{
			me->Redraw();
		}
		return 0;
	case WM_PAINT:
		me->OnPaint();
		return 0;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr(nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(nmhdr->code, (void*)lParam);
		}
		break;
	case WM_TIMER:
		me->EventTimerTick();
		break;
	case WM_SIZE:
		if (me)
		{
			UOSInt scnW = (UInt16)LOWORD(lParam);
			UOSInt scnH = (UInt16)HIWORD(lParam);
			if (me->drawBuff)
			{
				me->deng->DeleteImage(me->drawBuff);
			}
			me->drawBuff = me->deng->CreateImage32(Math::Size2D<UOSInt>(scnW, scnH), Media::AT_NO_ALPHA);
			me->drawBuff->SetHDPI(me->GetHDPI());
			me->drawBuff->SetVDPI(me->GetHDPI());
			me->UpdateScrollBar();
			me->Redraw();
		}
		break;//DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUITextView::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUITextView::TFVWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_IBEAM); 
    wc.hbrBackground = 0;//BGBRUSH; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUITextView::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::GUITextView::OnPaint()
{
	RECT rc;
	PAINTSTRUCT ps;
	GetClientRect((HWND)this->hwnd, &rc);
	if (this->drawBuff == 0)
	{	
		BeginPaint((HWND)this->hwnd, &ps);
		FillRect(ps.hdc, &rc, BGBRUSH);
		EndPaint((HWND)this->hwnd, &ps);
		return;
	}

	this->DrawImage(this->drawBuff);

	Media::GDIImage *img = (Media::GDIImage*)this->drawBuff;
	BeginPaint((HWND)this->hwnd, &ps);
	BitBlt(ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, (HDC)img->hdcBmp, 0, 0, SRCCOPY);
	EndPaint((HWND)this->hwnd, &ps);
	this->UpdateCaretPos();
}

void UI::GUITextView::UpdateScrollBar()
{
	if (this->drawFont == 0)
	{
		return;
	}

	Math::Size2DDbl sz;
	RECT rc;
	if (this->drawBuff == 0)
	{
		sz.y = 12;
	}
	else
	{
		Media::DrawFont *fnt = this->CreateDrawFont(this->drawBuff);
		if (fnt == 0)
		{
			sz.y = 12;
		}
		else
		{
			sz = this->drawBuff->GetTextSize(fnt, CSTR("Test"));
			this->drawBuff->DelFont(fnt);
		}
	}
	GetClientRect((HWND)this->hwnd, &rc);
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE;
	si.nPage = (UINT)Double2Int32((rc.bottom - rc.top) / sz.y);
	this->pageLineCnt = si.nPage;
	this->pageLineHeight = Double2Int32(sz.y);
	SetScrollInfo((HWND)this->hwnd, SB_VERT, &si, TRUE);

	si.nPage = (UINT)(rc.right - rc.left);
	SetScrollInfo((HWND)this->hwnd, SB_HORZ, &si, TRUE);
}

Bool UI::GUITextView::IsShiftPressed()
{
	Int16 shiftLock = (Int16)(GetKeyState(VK_LSHIFT) | GetKeyState(VK_RSHIFT));
	return (shiftLock & 0x80) != 0;
}

void UI::GUITextView::SetScrollHPos(UOSInt pos, Bool redraw)
{
	SetScrollPos((HWND)this->hwnd, SB_HORZ, (int)(OSInt)pos, redraw?TRUE:FALSE);
}

void UI::GUITextView::SetScrollVPos(UOSInt pos, Bool redraw)
{
	SetScrollPos((HWND)this->hwnd, SB_VERT, (int)(OSInt)pos, redraw?TRUE:FALSE);
}

void UI::GUITextView::SetScrollHRange(UOSInt min, UOSInt max)
{
	SetScrollRange((HWND)this->hwnd, SB_HORZ, (int)(OSInt)min, (int)(OSInt)max, TRUE);	
}

void UI::GUITextView::SetScrollVRange(UOSInt min, UOSInt max)
{
	SetScrollRange((HWND)this->hwnd, SB_VERT, (int)(OSInt)min, (int)(OSInt)max, TRUE);
}

UInt32 UI::GUITextView::GetCharCntAtWidth(WChar *str, UOSInt strLen, UOSInt pxWidth)
{
	if (this->drawBuff)
	{
		SIZE sz;
		Media::GDIFont *fnt = (Media::GDIFont*)this->CreateDrawFont(this->drawBuff);
		Media::GDIImage *img = (Media::GDIImage*)this->drawBuff;
		HDC hdc = (HDC)img->hdcBmp;
		SelectObject(hdc, (HFONT)fnt->hfont);
		Int32 textX;
		GetTextExtentExPoint(hdc, str, (Int32)(OSInt)strLen, (int)(OSInt)pxWidth, &textX, 0, &sz);
		this->drawBuff->DelFont(fnt);
		return (UInt32)textX;
	}
	else
	{
		SIZE sz;
		HDC hdc = GetDC((HWND)this->hwnd);
		void *fnt = this->GetFont();
		if (fnt)
		{
			SelectObject(hdc, fnt);
		}
		Int32 textX;
		GetTextExtentExPoint(hdc, str, (Int32)(OSInt)strLen, (int)(OSInt)pxWidth, &textX, 0, &sz);
		ReleaseDC((HWND)this->hwnd, hdc);
		return (UInt32)textX;
	}
}

void UI::GUITextView::GetDrawSize(WChar *str, UOSInt strLen, UOSInt *width, UOSInt *height)
{
	if (this->drawBuff)
	{
		Math::Size2DDbl sz;
		Media::DrawFont *fnt = this->CreateDrawFont(this->drawBuff);
		sz = ((Media::GDIImage*)this->drawBuff)->GetTextSize(fnt, str, (OSInt)strLen);
		*width = (UOSInt)Double2OSInt(sz.x);
		*height = (UOSInt)Double2OSInt(sz.y);
		this->drawBuff->DelFont(fnt);
	}
	else
	{
		SIZE sz;
		HDC hdc = GetDC((HWND)this->hwnd);
		void *fnt = this->GetFont();
		if (fnt)
		{
			SelectObject(hdc, fnt);
		}
		GetTextExtentExPoint(hdc, str, (Int32)(OSInt)strLen, 0, 0, 0, &sz);
		ReleaseDC((HWND)this->hwnd, hdc);
		*width = (UInt32)sz.cx;
		*height = (UInt32)sz.cy;
	}
}

void UI::GUITextView::SetCaretPos(OSInt scnX, OSInt scnY)
{
	::SetCaretPos((int)scnX, (int)scnY);
}

UI::GUITextView::GUITextView(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> deng) : UI::GUIControl(ui, parent)
{
	this->deng = deng;
	this->drawBuff = 0;
	this->pageLineCnt = 0;
	this->pageLineHeight = 12;

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, WS_EX_CONTROLPARENT, 0, 0, 200, 200);
	SetTimer((HWND)this->hwnd, 1, 1000, 0);

	this->bgColor = this->GetColorBg();
	this->scrColor = 0xffcccccc;
	this->txtColor = this->GetColorText();
	this->lineNumColor = this->GetColorTextAlt();
	this->selColor = this->GetColorHightlight();
	this->selTextColor = this->GetColorHightlightText();
}

UI::GUITextView::~GUITextView()
{
	KillTimer((HWND)this->hwnd, 1);
	if (this->drawBuff)
	{
		this->deng->DeleteImage(this->drawBuff);
		this->drawBuff = 0;
	}
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst());
	}
}

Text::CString UI::GUITextView::GetObjectClass()
{
	return CSTR("TextFileView");
}

OSInt UI::GUITextView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITextView::UpdateFont()
{
	this->drawFont = this->GetFont();
	UpdateScrollBar();
	this->Redraw();
}

OSInt UI::GUITextView::GetScrollHPos()
{
	return ::GetScrollPos((HWND)this->hwnd, SB_HORZ);
}

OSInt UI::GUITextView::GetScrollVPos()
{
	return ::GetScrollPos((HWND)this->hwnd, SB_VERT);
}
