#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include "UI/GUILabel.h"

#include <windows.h>

UI::GUILabel::GUILabel(UI::GUICore *ui, UI::GUIClientControl *parent, Text::CString initText) : UI::GUIControl(ui, parent)
{
	this->hasTextColor = false;
	this->textColor = 0;
	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, L"STATIC", initText.v, style, 0, 0, 0, 200, 24);
}

UI::GUILabel::~GUILabel()
{
}

void UI::GUILabel::SetText(Text::CString text)
{
	const WChar *wptr = Text::StrToWCharNew(text.v);
	SetWindowTextW((HWND)this->hwnd, wptr);
	Text::StrDelNew(wptr);
}

Text::CString UI::GUILabel::GetObjectClass()
{
	return CSTR("Label");
}

OSInt UI::GUILabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::GUILabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::GUILabel::GetTextColor()
{
	return this->textColor;
}

void UI::GUILabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
}
