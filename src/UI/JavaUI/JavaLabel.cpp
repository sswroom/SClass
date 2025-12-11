#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JavaLabel.h"

UI::JavaUI::JavaLabel::JavaLabel(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUILabel(ui, parent)
{
	NEW_CLASSNN(this->lbl, ::Java::JavaJLabel(label));
}

UI::JavaUI::JavaLabel::~JavaLabel()
{
	this->lbl.Delete();
}

void UI::JavaUI::JavaLabel::SetText(Text::CStringNN text)
{
}

OSInt UI::JavaUI::JavaLabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::JavaUI::JavaLabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::JavaUI::JavaLabel::GetTextColor()
{
	return this->textColor;
}

void UI::JavaUI::JavaLabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
}

