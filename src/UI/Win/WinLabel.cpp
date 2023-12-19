#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinLabel.h"

#include <windows.h>

UI::Win::WinLabel::WinLabel(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText) : UI::GUILabel(ui, parent)
{
	this->hasTextColor = false;
	this->textColor = 0;
	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"STATIC", initText.v, style, 0, 0, 0, 200, 24);
}

UI::Win::WinLabel::~WinLabel()
{
}

void UI::Win::WinLabel::SetText(Text::CStringNN text)
{
	const WChar *wptr = Text::StrToWCharNew(text.v);
	SetWindowTextW((HWND)this->hwnd, wptr);
	Text::StrDelNew(wptr);
}

OSInt UI::Win::WinLabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::Win::WinLabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::Win::WinLabel::GetTextColor()
{
	return this->textColor;
}

void UI::Win::WinLabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
}
