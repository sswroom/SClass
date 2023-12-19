#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinTextBox.h"

#include <windows.h>
#ifndef _WIN32_WCE
#include <richedit.h>
#endif

UI::Win::WinTextBox::WinTextBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText) : UI::GUITextBox(ui, parent)
{
	UInt32 style = WS_BORDER | WS_TABSTOP | WS_CHILD | ES_AUTOHSCROLL;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"EDIT", initText.v, style, WS_EX_CLIENTEDGE, 0, 0, 200, 28);
}

UI::Win::WinTextBox::WinTextBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline) : UI::GUITextBox(ui, parent)
{
	UInt32 style = WS_BORDER | WS_TABSTOP | WS_CHILD | ES_AUTOHSCROLL;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	if (isMultiline)
	{
		style |= ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"EDIT", initText.v, style, WS_EX_CLIENTEDGE, 0, 0, 200, 28);
}

UI::Win::WinTextBox::~WinTextBox()
{
}

void UI::Win::WinTextBox::SetReadOnly(Bool isReadOnly)
{
	SendMessage((HWND)this->hwnd, EM_SETREADONLY, isReadOnly?TRUE:FALSE, 0);
}

void UI::Win::WinTextBox::SetPasswordChar(UTF32Char c)
{
	SendMessage((HWND)this->hwnd, EM_SETPASSWORDCHAR, c, 0);
}

void UI::Win::WinTextBox::SetText(Text::CStringNN txt)
{
	const WChar *wptr = Text::StrToWCharNew(txt.v);
	SetWindowTextW((HWND)hwnd, wptr);
	Text::StrDelNew(wptr);
}

UTF8Char *UI::Win::WinTextBox::GetText(UTF8Char *buff)
{
	UOSInt leng = (UOSInt)GetWindowTextLengthW((HWND)hwnd);
	WChar *wptr = MemAlloc(WChar, leng + 1);
	GetWindowTextW((HWND)hwnd, wptr, (int)(leng + 1));
	buff = Text::StrWChar_UTF8(buff, wptr);
	MemFree(wptr);
	return buff;
}

Bool UI::Win::WinTextBox::GetText(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UOSInt leng = (UOSInt)GetWindowTextLengthW((HWND)hwnd);
	WChar *wptr = MemAlloc(WChar, leng + 1);
	GetWindowTextW((HWND)hwnd, wptr, (int)leng + 1);
	sb->AppendW(wptr);
	MemFree(wptr);
	return true;
}

OSInt UI::Win::WinTextBox::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case EN_CHANGE:
		EventTextChange();
		break;
	}
	return 0;
}

void UI::Win::WinTextBox::SetWordWrap(Bool wordWrap)
{
/*	if (wordWrap)
	{
		SendMessage((HWND)this->hwnd, EM_SETWORDBREAKPROC, 0, GUITextBox_WordBreakProc);
	}*/
}

void UI::Win::WinTextBox::SelectAll()
{
	SendMessage((HWND)this->hwnd, EM_SETSEL, 0, -1);
}
