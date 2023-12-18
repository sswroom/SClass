#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include "UI/Win/WinButton.h"

#include <windows.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt UI::Win::WinButton::nextId = 100;

OSInt __stdcall UI::Win::WinButton::BTNWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::Win::WinButton *me = (UI::Win::WinButton*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		me->EventButtonDown();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	case WM_LBUTTONUP:
		me->EventButtonUp();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	case WM_KILLFOCUS:
		me->EventFocusLost();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	case WM_SETFOCUS:
		me->EventFocus();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	default:
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::Win::WinButton::WinButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN txt) : UI::GUIButton(ui, parent)
{
	UInt32 style = WS_CHILD | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style |= WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui.Ptr())->GetHInst(), parent, L"BUTTON", txt.v, style, 0, 0, 0, 200, 24);
#ifndef _WIN32_WCE
	SetWindowLongPtr((HWND)this->hwnd, GWLP_ID, (Int32)(this->btnId = nextId++));
#else
	this->btnId = nextId++;
#endif

#ifdef _WIN32_WCE
	this->oriWndProc = (void*)UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWL_WNDPROC, (OSInt)BTNWndProc);
#else
	this->oriWndProc = (void*)UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)BTNWndProc);
#endif
}

UI::Win::WinButton::~WinButton()
{
#ifdef _WIN32_WCE
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWL_WNDPROC, (OSInt)this->oriWndProc);
#else
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->oriWndProc);
#endif
}

void UI::Win::WinButton::SetText(Text::CStringNN text)
{
	const WChar *wptr = Text::StrToWCharNew(text.v);
	SetWindowTextW((HWND)this->hwnd, wptr);
	Text::StrDelNew(wptr);
}

void UI::Win::WinButton::SetFont(const UTF8Char *name, UOSInt nameLen, Double fontHeightPt, Bool isBold)
{
	SDEL_STRING(this->fontName);
	if (name)
	{
		this->fontName = Text::String::New(name, nameLen).Ptr();
	}
	this->fontHeightPt = fontHeightPt;
	this->fontIsBold = isBold;
	InitFont();
}

OSInt UI::Win::WinButton::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case BN_CLICKED:
		this->EventButtonClick();
		break;
	case BN_DBLCLK:
		break;
/*	case BN_DISABLE:
		break;*/
/*	case BN_PUSHED:
		break;
	case BN_UNPUSHED:
		break;*/
	case BN_SETFOCUS:
		break;
	case BN_KILLFOCUS:
		break;
	}
	return 0;
}

OSInt UI::Win::WinButton::GetBtnId()
{
	return this->btnId;
}

void UI::Win::WinButton::SetDefaultBtnLook()
{
	SendMessage((HWND)this->hwnd, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);
}
