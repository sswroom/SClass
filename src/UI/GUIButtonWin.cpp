#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIButton.h"
#include "UI/GUICoreWin.h"

#include <windows.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt UI::GUIButton::nextId = 100;

OSInt __stdcall UI::GUIButton::BTNWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::GUIButton *me = (UI::GUIButton*)UI::GUICoreWin::MSGetWindowObj(hWnd, GWL_USERDATA);
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		me->EventButtonDown();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	case WM_LBUTTONUP:
		me->EventButtonUp();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	case WM_KILLFOCUS:
		me->OnFocusLost();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	case WM_SETFOCUS:
		me->OnFocus();
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	default:
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::GUIButton::GUIButton(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *txt) : UI::GUIControl(ui, parent)
{
	Int32 style = WS_CHILD | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style |= WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, L"BUTTON", txt, style, 0, 0, 0, 200, 24);
	NEW_CLASS(this->btnClkHandlers, Data::ArrayList<UIEvent>());
	NEW_CLASS(this->btnClkHandlersObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->btnUpDownHandlers, Data::ArrayList<UpDownEvent>());
	NEW_CLASS(this->btnUpDownHandlersObjs, Data::ArrayList<void *>());
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

UI::GUIButton::~GUIButton()
{
#ifdef _WIN32_WCE
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWL_WNDPROC, (OSInt)this->oriWndProc);
#else
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->oriWndProc);
#endif
	DEL_CLASS(this->btnClkHandlers);
	DEL_CLASS(this->btnClkHandlersObjs);
	DEL_CLASS(this->btnUpDownHandlers);
	DEL_CLASS(this->btnUpDownHandlersObjs);
}

void UI::GUIButton::SetText(const UTF8Char *text)
{
	const WChar *wptr = Text::StrToWCharNew(text);
	SetWindowTextW((HWND)this->hwnd, wptr);
	Text::StrDelNew(wptr);
}

void UI::GUIButton::SetFont(const UTF8Char *name, Double fontHeightPt, Bool isBold)
{
	SDEL_TEXT(this->fontName);
	if (name)
	{
		this->fontName = Text::StrCopyNew(name);
	}
	this->fontHeightPt = fontHeightPt;
	this->fontIsBold = isBold;
	InitFont();
}

const UTF8Char *UI::GUIButton::GetObjectClass()
{
	return (const UTF8Char*)"BUTTON";
}

OSInt UI::GUIButton::OnNotify(UInt32 code, void *lParam)
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

void UI::GUIButton::OnFocus()
{
}

void UI::GUIButton::OnFocusLost()
{
}

void UI::GUIButton::EventButtonClick()
{
	UOSInt i;
	i = this->btnClkHandlers->GetCount();
	while (i-- > 0)
	{
		this->btnClkHandlers->GetItem(i)(this->btnClkHandlersObjs->GetItem(i));
	}
}

void UI::GUIButton::EventButtonDown()
{
	UOSInt i;
	i = this->btnUpDownHandlers->GetCount();
	while (i-- > 0)
	{
		this->btnUpDownHandlers->GetItem(i)(this->btnUpDownHandlersObjs->GetItem(i), true);
	}
}

void UI::GUIButton::EventButtonUp()
{
	UOSInt i;
	i = this->btnUpDownHandlers->GetCount();
	while (i-- > 0)
	{
		this->btnUpDownHandlers->GetItem(i)(this->btnUpDownHandlersObjs->GetItem(i), false);
	}
}

OSInt UI::GUIButton::GetBtnId()
{
	return this->btnId;
}

void UI::GUIButton::SetDefaultBtnLook()
{
	SendMessage((HWND)this->hwnd, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);
}

void UI::GUIButton::HandleButtonClick(UIEvent handler, void *userObj)
{
	this->btnClkHandlers->Add(handler);
	this->btnClkHandlersObjs->Add(userObj);
}

void UI::GUIButton::HandleButtonUpDown(UpDownEvent handler, void *userObj)
{
	this->btnUpDownHandlers->Add(handler);
	this->btnUpDownHandlersObjs->Add(userObj);
}
