#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/Java/JavaLabel.h"

UI::Java::JavaLabel::JavaLabel(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUILabel(ui, parent)
{
}

UI::Java::JavaLabel::~JavaLabel()
{
}

void UI::Java::JavaLabel::SetText(Text::CStringNN text)
{
}

OSInt UI::Java::JavaLabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::Java::JavaLabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::Java::JavaLabel::GetTextColor()
{
	return this->textColor;
}

void UI::Java::JavaLabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
}

