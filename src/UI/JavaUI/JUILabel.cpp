#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUILabel.h"

UI::JavaUI::JUILabel::JUILabel(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUILabel(ui, parent)
{
	NEW_CLASSNN(this->lbl, Java::JavaJLabel(label));
}

UI::JavaUI::JUILabel::~JUILabel()
{
	this->lbl.Delete();
}

void UI::JavaUI::JUILabel::SetText(Text::CStringNN text)
{
}

OSInt UI::JavaUI::JUILabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::JavaUI::JUILabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::JavaUI::JUILabel::GetTextColor()
{
	return this->textColor;
}

void UI::JavaUI::JUILabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
}

